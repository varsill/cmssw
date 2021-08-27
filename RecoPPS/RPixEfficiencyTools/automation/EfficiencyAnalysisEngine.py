import automation_control as ctrl
import argparse
import subprocess
import enum 
import inspect
from copy import deepcopy as dcopy
import logging
import sys
from typing import Any, Type, Dict, Union


logger = logging.getLogger("EfficiencyAnalysisLogger")
logger.setLevel(logging.DEBUG)

ch = logging.FileHandler("EfficiencyAnalysisEngine.log")
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
ch.setFormatter(formatter)
logger.addHandler(ch)

ch = logging.StreamHandler()
ch.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
ch.setFormatter(formatter)
logger.addHandler(ch)

campaign='test_campaign2'
workflow='test_workflow'
dataset = "/Charmonium/Run2018B-12Nov2019_UL2018-v1/AOD"
template = "CrabConfigTemplate.py"


class AnyClass():
    """Class for performing generic comparision
       This class allows defining two types of objects: 
       1) an object which will always return True in case of 
          comparing any other object with this object (this happens when no argument is passed to the constructor)
       2) an object which will return True if the type of an object being compared
          is one of the types specified while constructing the comparison object, False otherwise
    """
    def __eq__(self, other: Any)->bool:
        """Used for operator-style comparison ('==' operator)

        Args:
            other (Any): object to be compared

        Returns:
            bool: True if no types where specified in the constructor or when the type of the object being compared
                  is one of the types specified in the constructor, False otherwise
        """
        if not self.types: 
            return True
        for t in self.types:
            if t==type(other):
                return True
        return False

    def __init__(self, *types):
        """Constructor of the class, allows to create and object of 1) and 2) type

        Args:
            *args: List of types 
        """
        self.types=types

AnyInt = AnyClass(int)


def define_status_enum(TaskStatusClass: Type[enum.Enum])->Type[enum.Enum]:
    """Decorator used for marking the given class as the status enum and adding special __statuses__ member 
       which is the list of all the enum fields defined within it.
       The class provided as an input must inherit from enum.Enum class

    Args:
        TaskStatusClass (Type[enum.Enum]): class to be marked as status enum

    Returns:
        Type[enum.Enum]: class marked as status enum, with __statuses__ special member
    """
    TaskStatusClass.__statuses__ = TaskStatusClass.__members__.keys()
    return TaskStatusClass

def decorate_with_enum(TaskStatusClass: Type[Any])->Type[Any]:
    """Decorator for defining the TaskStatus class  
        
        Usage:
            decorate_with_enum(<enum status class defined with @define_status_enum decorator>)
       
        As a result we get the class consisting of all the user defined fields which were present in the input class
        and the status enum fields defined in given enum status class.
        The result class, apart from the __statuses__ special field has another special field - __allmembers__
        consisting of all user-defined member fields (both from the input class and the enum status class)
    """
    def wrapper(AdditionalClass: Type[]):
        TaskStatusClass.__allmembers__=[status for status in TaskStatusClass.__statuses__]
        for attr in AdditionalClass.__dict__:
            if not attr.startswith("_"):
             
                setattr(TaskStatusClass, attr, AdditionalClass.__dict__[attr])
                TaskStatusClass.__allmembers__.append(attr)
        return TaskStatusClass
    return wrapper


@define_status_enum
class TaskStatusEnum(enum.Enum):
    """
        Class to encode enum tasks statuses for the purpouse of this automation workflow
    """
    initialized = enum.auto(),
    duringFirstWorker = enum.auto(),
    waitingForFirstWorkerTransfer= enum.auto(),
    duringFirstHarvester = enum.auto(),
    waitingForFirstHarvester = enum.auto(),
    afterFirstHarvester = enum.auto(),
    duringSecondWorker = enum.auto(),
    afterSecondWorker= enum.auto(),
    duringSecondHarvester = enum.auto()
    afterSecondHarvester = enum.auto(),
    done = enum.auto()
    

@decorate_with_enum(TaskStatusEnum)
class TaskStatus:
    loop_id = 0.0    
    condor_job_id = 0

def get_tasks_numbers_list(tasks_list_path):
    with open(tasks_list_path) as tasks_list_path:
        tasks_list_data = tasks_list_path.read()
        tasks_list_data = tasks_list_data.replace(" ", "")
        tasks_list = tasks_list_data.split(",")
    return tasks_list


def prepare_parser()->argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=
    """This is a script to run PPS Efficiency Analysis automation workflow""", formatter_class=argparse.RawTextHelpFormatter)
    
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
    except Exception as e:
        logger.warning("Exception thrown: "+str(e))
        return False
    
    
def get_status(task_information, TaskStatusClass):
    for member_name in TaskStatusClass.__statuses__:
        if task_information.get(member_name)==1:
            return member_name
    raise Exception("Task is in neither of the states defined in TaskStatusClass")


def submit_task_to_crab(campaign, workflow, data_period, dataset, template):
    result = ctrl.submit_task_to_crab(campaign, workflow, data_period, get_runs_range(data_period), template, dataset)
    return result


def set_status(task_status, operation_result):
    task_status.duringFirstWorker=1
    task_status.initialized=0
    task_status.loop_id+=1
    return task_status


executable = """
                cmsRun /afs/cern.ch/user/l/lkita/CMSSW_11_3_2/src/RecoPPS/RPixEfficiencyTools/python/EfficiencyAnalysisDQMHarvester_cfg.py 
                inputFileName=/eos/user/l/lkita/Charmonium/crab_test_campaign2_test_workflow_12/210825_131619/0000/tmp_1.root 
                outputDirectoryPath=/afs/cern.ch/user/l/lkita/CMSSW_11_3_2/src/RecoPPS/RPixEfficiencyTools/OutputFiles/
                bunchSelection=NoSelection
             """

def submit_task_to_condor(campaign, workflow, data_period):
    return ctrl.submit_task_to_condor(campaign, workflow, data_period, executable)


def set_status_during_first_harvester(task_status, cluster_id):
    task_status.waitingForFirstHarvester=1
    task_status.duringFirstHarvester=0
    task_status.condor_job_id=cluster_id
    return task_status


def wait_for_condor(campaign, workflow, data_period):
    task_controller = ctrl.TaskCtrl.TaskControl(campaign=campaign, workflow=workflow, TaskStatusClass=TaskStatusClass)
    last_task_status = task_controller.getLastTask(data_period=data_period)
    cluster_id = int(last_task_status.get("condor_job_id"))
    return ctrl.wait_for_condor(cluster_id)



TRANSITIONS_DICT = {
                        'initialized': (submit_task_to_crab, 0, set_status, [dataset, template] ),
                        'duringFirstWorker': (ctrl.check_if_crab_task_is_finished, True, TaskStatus.waitingForFirstWorkerTransfer, []),
                        'waitingForFirstWorkerTransfer': (is_already_transfered, True, TaskStatus.duringFirstHarvester, []),
                        'duringFirstHarvester': (submit_task_to_condor, AnyInt, set_status_during_first_harvester, []),
                        'waitingForFirstHarvester': (wait_for_condor, True, TaskStatus.afterFirstHarvester, [])
                   } 


TaskInformation = Dict[Union[int, float, str]] #record from InfluxDB's "task" table
TaskStatus = Type[Any] #class defined with decorate_with_enum() decorator

def perform_action(task_information: TaskInformation, task_controller: ctrl.TaskCtrl.TaskControl, TaskStatusClass: TaskStatus)->Union[None, TaskInformation]:
    """Attempts to proceed on automation workflow defined within TRANSITIONS_DICT

    Args:
        task_information (TaskInformation): current task status 
        task_controller (ctrl.TaskCtrl.TaskControl): reference to task controller created for the purpouse of database connection etc.
        TaskStatusClass (TaskStatus): task status class - class defined with decorate_with_enum() decorator (should be corresponding to the task_information record)

    Returns:
        Union[None, TaskInformation]: New TaskInformation with the updated status after the successfull operation, None otherwise
    """
    try:   
        
        func, expected_result, next_status, parms = TRANSITIONS_DICT[get_status(task_information, TaskStatusClass)]
        logger.debug("Method: %s will be performed on: (campaign: %s, workflow: %s, data period: %s)" %(func.__name__, task_information['campaign'], task_information['workflow'], task_information['dataPeriod']))
        result = func(task_information['campaign'], task_information['workflow'], task_information['dataPeriod'], *parms)
        
        if result==expected_result:
            logger.debug("The operation result is the same as expected.")
            if callable(next_status):
                logger.debug("Setting status with status generator")
                task_information = task_controller.setStatusWithStatusGenerator(task_information['dataPeriod'], next_status, result)
            else:
                logger.debug("Setting status as: "+str(next_status))
                task_information=task_controller.setStatus(task_information['dataPeriod'], next_status)
            return task_information
        return None
    except Exception as e:
        logger.warning("Exception thrown: "+str(e))
        return None
    

def process_tasks(task_controller: ctrl.TaskCtrl.TaskControl, TaskStatusClass: TaskStatus)->None:
    """Attempts to perform as many steps in automation control as possible. Stops if perform_action() returns None (meaning that its operation didn't finish successfully)

    Args:
        task_controller (ctrl.TaskCtrl.TaskControl): task controller managing database connection etc.
        TaskStatusClass (TaskStatus): task status class - class defined with decorate_with_enum() decorator
    """
    
    tasks_in_database_information_list = task_controller.getAllTasks().get_points()
    for task_information in tasks_in_database_information_list:
        logger.debug("Processing task: "+str(task_information))
        task_information_buffer = task_information
        while task_information_buffer != None:
            task_information_buffer  = perform_action(task_information_buffer, task_controller, TaskStatusClass=TaskStatusClass)
    
    
if __name__ == '__main__':
    parser = prepare_parser()
    opts = parser.parse_args()
    task_controller = ctrl.TaskCtrl.TaskControl(campaign=campaign, workflow=workflow, TaskStatusClass=TaskStatusClass)
    process_new_tasks(tasks_list_path, task_controller)
    process_tasks(task_controller, TaskStatusClass=TaskStatus)
    
    
    