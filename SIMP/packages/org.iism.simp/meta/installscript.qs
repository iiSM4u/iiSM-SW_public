function Component()
{
    // 기본 생성자
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/SIMP.exe", "@StartMenuDir@/SIMP.lnk");
        
        // 제거 기능을 시작 메뉴에 추가
        component.addOperation("CreateShortcut", 
            "@TargetDir@/maintenancetool.exe", 
            "@StartMenuDir@/Uninstall SIMP.lnk",
            "workingDirectory=@TargetDir@",
            "iconPath=%SystemRoot%/system32/SHELL32.dll",
            "iconId=2");
    }
}