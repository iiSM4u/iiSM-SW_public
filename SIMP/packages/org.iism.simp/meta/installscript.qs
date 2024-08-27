function Component()
{
    // 기본 생성자
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") 
    {
        // 시작 메뉴에 바로가기 생성
        component.addOperation("CreateShortcut", "@TargetDir@/SIMP.exe", "@StartMenuDir@/SIMP.lnk");

        // 제거 기능을 시작 메뉴에 추가
        component.addOperation("CreateShortcut", 
            "@TargetDir@/maintenancetool.exe", 
            "@StartMenuDir@/Uninstall SIMP.lnk",
            "workingDirectory=@TargetDir@",
            "iconPath=%SystemRoot%/system32/SHELL32.dll",
            "iconId=2");

        // 바탕화면에 바로가기 생성
        var desktopDir = installer.value("UserDesktopDir");
        component.addOperation("CreateShortcut", 
            "@TargetDir@/SIMP.exe", 
            desktopDir + "/SIMP.lnk",
            "workingDirectory=@TargetDir@",
            "iconPath=@TargetDir@/SIMP.exe", 
            "iconId=0",
            "description=SIMP Application");
    }
}
