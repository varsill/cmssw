import controllers as ctrl
import argparse
import subprocess
import enum 

campaign='test_campaign2'
workflow='test_workflow'
dataset = "/Charmonium/Run2018B-12Nov2019_UL2018-v1/AOD"
template = "CrabConfigTemplate.py"


class TaskStatus(ctrl.TaskStatus, enum.Enum):
    """
    Simple class to encode tasks statuses
    """

    duringFirstWorker = enum.auto(),
    afterFirstWorker= enum.auto(),
    duringFirstHarvester = enum.auto()
    afterFirstHarvester = enum.auto(),
    duringSecondWorker = enum.auto(),
    afterSecondWorker= enum.auto(),
    duringSecondHarvester = enum.auto()
    afterSecondHarvester = enum.auto(),
    done = enum.auto()
    



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


def submit_task_to_crab(task_information):
    result = subprocess.run(['python', 'CrabSubmitter.py', '--crabCmd', 'submit', '-d', dataset, 
    '-r', get_runs_range(task_information['dataPeriod']),'-t', template, '-c', task_information['campaign'], 
    '-w', task_information['workflow'], '--dataPeriod', task_information['dataPeriod']], stderr=subprocess.STDOUT)
    return result

def wait_for_crab_task_to_finish(task_information):
    result = subprocess.run(['./automation_control/ecalautomation.py', '-c', task_information['campaign'], 
    '-w', task_information['workflow'], '--dataPeriod', task_information['dataPeriod'], 'wait', 
    "--resubcmd", "crab resubmit --proxy=/afs/cern.ch/user/e/ecalgit/grid_proxy.x509 crab_wdir", 
    "--howManyAttempts", "1", "-s", "0" ], stderr=subprocess.STDOUT)
    return result


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
    


    
if __name__ == '__main__':

    parser = prepare_parser()
    opts = parser.parse_args()
    
    task_controller = ctrl.TaskControl(campaign=campaign, workflow=workflow, TaskStatusClass=TaskStatus)
    
    #PROCESS NEW TASKS FROM .txt file
    process_new_tasks(opts.tasks_list_path, task_controller)
    
    #PROCESS TASKS WHICH ARE ALREADY IN DB
    
    tasks_in_database_information_list = task_controller.getAllTasks().get_points()
    
    for task_information in tasks_in_database_information_list:
        
        print(task_information)
        
        if task_information['initialized']==1:
            result = submit_task_to_crab(task_information)
            if result.returncode==0:
                task_controller.setStatus(task_information['dataPeriod'], TaskStatus.duringFirstWorker)
           
        #FIRST STEP
        
        elif task_information['duringFirstWorker']==1:
            result = wait_for_crab_task_to_finish(task_information) 
            if result.returncode:
                task_controller.setStatus(task_information['dataPeriod'], TaskStatus.afterFirstWorker)
        
        elif task_information['afterFirstWorker']==1:
            print("AFTER FIRST WORKER")
            if is_already_transfered(campaign, workflow, task_information['dataPeriod']):
                task_controller.setStatus(task_information['dataPeriod'], TaskStatus.duringFirstHarvester) 
            
        elif task_information['duringFirstHarvester']==1:
            print("DURING FIRST HARVESTER")

        elif task_information['duringFirstHarvester']==1:
            pass
        
        elif task_information['afterFirstHarvester']==1:
            pass
            
        #SECOND STEP    
        
        elif task_information['duringSecondWorker']==1:
            pass
        
        elif task_information['afterSecondWorker']==1:
            pass
            
        elif task_information['duringSecondHarvester']==1:
            pass

        elif task_information['duringSecondHarvester']==1:
            pass
        
        elif task_information['afterSecondHarvester']==1:
            pass
        
        elif task_information['done']==1:
            pass
    
    