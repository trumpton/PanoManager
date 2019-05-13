
function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", 
		"@TargetDir@/maintenancetool.exe", 
		"@StartMenuDir@/PanoManagerMaintenanceTool.lnk",
            	"workingDirectory=@TargetDir@", 
		"iconPath=@TargetDir@/maintenancetool.exe",
            	"iconId=1", "description=Uninstall / Modify Panorama Manager");
    }
}
