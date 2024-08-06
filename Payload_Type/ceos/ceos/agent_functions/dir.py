from mythic_container.MythicCommandBase import *
import json
from mythic_container.MythicRPC import *


class DirArguments(TaskArguments):
    def __init__(self, command_line, **kwargs):
        super().__init__(command_line, **kwargs)
        self.args = [] # no arguments needed

    async def parse_arguments(self):
        pass


class DirCommand(CommandBase):
    cmd = "dir"
    needs_admin = False
    help_cmd = "dir"
    description = "Provides dir listing"
    version = 1
    author = "@Ferrevdv"
    attackmapping = []
    argument_class = ExitThreadArguments
    attributes = CommandAttributes(
        builtin=True
    )

    async def create_go_tasking(self, taskData: MythicCommandBase.PTTaskMessageAllData) -> MythicCommandBase.PTTaskCreateTaskingMessageResponse:
        response = MythicCommandBase.PTTaskCreateTaskingMessageResponse(
            TaskID=taskData.Task.ID,
            Success=True,
        )
        # await SendMythicRPCArtifactCreate(MythicRPCArtifactCreateMessage(
        #     TaskID=taskData.Task.ID,
        #     ArtifactMessage=f"$.NSApplication.sharedApplication.terminate",
        #     BaseArtifactType="API"
        # ))
        return response

    async def process_response(self, task: PTTaskMessageAllData, response: any) -> PTTaskProcessResponseMessageResponse:
        resp = PTTaskProcessResponseMessageResponse(TaskID=task.Task.ID, Success=True)
        return resp
