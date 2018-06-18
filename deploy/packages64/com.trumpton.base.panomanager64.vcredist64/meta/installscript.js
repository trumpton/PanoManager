
function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
		component.addOperation("Execute",
		"@TargetDir@/redist/mu_visual_cpp_2015_redistributable_update_3_x64_9052538.exe") ;
    }
}
