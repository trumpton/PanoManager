
function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", 
		"@TargetDir@/bin/PanoManager.exe", 
		"@StartMenuDir@/PanoManager.lnk",
            	"workingDirectory=@TargetDir@", 
		"iconPath=@TargetDir@/bin/PanoManager.exe",
            	"iconId=1", "description=Panorama Manager");
        component.addOperation("CreateShortcut", 
		"@TargetDir@/maintenancetool.exe", 
		"@StartMenuDir@/PanoManagerMaintenanceTool.lnk",
            	"workingDirectory=@TargetDir@", 
		"iconPath=@TargetDir@/bin/PanoManager.exe",
            	"iconId=1", "description=Uninstall / Modify Panorama Manager");
    }
}
