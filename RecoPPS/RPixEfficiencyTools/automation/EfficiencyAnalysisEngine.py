import automation_control as ctrl
import argparse
import subprocess
import enum 
import inspect
from copy import deepcopy as dcopy

campaign='test_campaign2'
workflow='test_workflow'
dataset = "/Charmonium/Run2018B-12Nov2019_UL2018-v1/AOD"
template = "CrabConfigTemplate.py"

def define_enum(TaskStatusClass):
    TaskStatusClass.__statuses__ = TaskStatusClass.__members__.keys()
    return TaskStatusClass

def decorate(TaskStatusClass):
    def wrapper(AdditionalClass):
        TaskStatusClass.__allmembers__=[status for status in TaskStatusClass.__statuses__]
        for attr in AdditionalClass.__dict__:
            if not attr.startswith("_"):
             
                setattr(TaskStatusClass, attr, AdditionalClass.__dict__[attr])
                TaskStatusClass.__allmembers__.append(attr)
        return TaskStatusClass
    return wrapper


@define_enum
class TaskStatusEnum(enum.Enum):
    """
    Simple class to encode tasks statuses
    """
    initialized = enum.auto(),
    duringFirstWorker = enum.auto(),
    waitingForFirstWorkerTransfer= enum.auto(),
    duringFirstHarvester = enum.auto()
    afterFirstHarvester = enum.auto(),
    duringSecondWorker = enum.auto(),
    afterSecondWorker= enum.auto(),
    duringSecondHarvester = enum.auto()
    afterSecondHarvester = enum.auto(),
    done = enum.auto()
    

@decorate(TaskStatusEnum)
class TaskStatus:
    loop_id = 0.0    


def get_tasks_numbers_list(tasks_list_path):
    with open(tasks_list_path) as tasks_list_path:
        tasks_list_data = tasks_list_path.read()
        tasks_list_data = tasks_list_data.replace(" ", "")
        tasks_list = tasks_list_data.split(",")
    return tasks_list


def prepare_parser():
    parser = argparse.ArgumentParser(description=
    """ """, formatter_class=argparse.RawTextHelpFormatter)
    
    parser.add_argument('-t', '--tasks_list', dest='tasks_list_path', help='path to file containing list of data periods', required=True)   
    return parser


def get_runs_range(data_period):
    """MOCKED"""
    return '317080'





def process_new_tasks(tasks_list_path, task_controller):
    tasks_list = get_tasks_numbers_list(tasks_list_path)
    tasks_list = set(tasks_list)
    tasks_in_database = task_controller.getAllTasks().get_points()
    tasks_in_database = set(map(lambda x: x['dataPeriod'], tasks_in_database))
    tasks_not_submited_yet = tasks_list-tasks_in_database
    if tasks_not_submited_yet:
        task_controller.submitTasks(tasks_not_submited_yet)


def flatten(t):
     return [item for sublist in t for item in sublist]
   
   
def is_already_transfered(campaign, workflow, dataPeriod):
    
    try:
        process = subprocess.Popen(["crab", "status", "crab_"+"_".join([campaign, workflow, str(dataPeriod)])], stdout=subprocess.PIPE)
        stdout = process.communicate()[0]

        stdout = str(stdout, "utf-8")
        
        status = list(filter(lambda x: x!='', flatten(map(lambda x: x.split("\t"), stdout.split("\n")[8].split(" ")))))[2]
        
        if status=="finished":
            return True
        return False
    except Exception:
        return False
    
    
def get_status(task_information, TaskStatusClass):
    for member_name in TaskStatusClass.__statuses__:
        if task_information.get(member_name)==1:
            return member_name
    raise Exception("Task is in neither of the states defined in TaskStatusClass")


def submit_task_to_crab(campaign, workflow, data_period, dataset, template):
    result = ctrl.submit_task_to_crab(campaign, workflow, data_period, get_runs_range(data_period), template, dataset)
    return result


def set_status(task_status):
    task_status.duringFirstWorker=1
    task_status.initialized=0
    task_status.loop_id+=1
    return task_status


transition_dict = {
                        'initialized': (submit_task_to_crab, 0, set_status, [dataset, template] ),
                        'duringFirstWorker': (ctrl.check_if_crab_task_is_finished, True, TaskStatus.waitingForFirstWorkerTransfer, []),
                        'waitingForFirstWorkerTransfer': (is_already_transfered, True, TaskStatus.duringFirstHarvester, []),
                        'duringFirstHarvester': (lambda x,y,z : None, True, TaskStatus.duringFirstHarvester, [])
                  } 



def perform_action(task_information, task_controller, TaskStatusClass):   
    func, expected_result, next_status, parms = transition_dict[get_status(task_information, TaskStatusClass)]
    print("START", func.__name__)
    result = func(task_information['campaign'], task_information['workflow'], task_information['dataPeriod'], *parms)
    print("END", result)
    if result==expected_result:
       if callable(next_status):
        task_information = task_controller.setStatusWithStatusGenerator(task_information['dataPeriod'], next_status)
       else:
        task_information=task_controller.setStatus(task_information['dataPeriod'], next_status)
       return task_information
    return None
    
    
def process_tasks(opts, TaskStatusClass):
    task_controller = ctrl.TaskCtrl.TaskControl(campaign=campaign, workflow=workflow, TaskStatusClass=TaskStatusClass)
    process_new_tasks(opts.tasks_list_path, task_controller)
    tasks_in_database_information_list = task_controller.getAllTasks().get_points()
    for task_information in tasks_in_database_information_list:
        print(task_information)
        task_information_buffer = task_information
        while task_information_buffer != None:
            task_information_buffer  = perform_action(task_information_buffer, task_controller, TaskStatusClass=TaskStatusClass)
    
    
if __name__ == '__main__':

    parser = prepare_parser()
    opts = parser.parse_args()
    
    process_tasks(opts, TaskStatusClass=TaskStatus)
    
    
    