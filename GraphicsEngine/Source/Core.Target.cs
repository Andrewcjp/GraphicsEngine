
using EngineBuildTool;

class CoreTargetRules : TargetRules
{
    bool BuildPhysx = false;
    bool BuildVulkan = false;
    bool BuildCSharp = false;
    public CoreTargetRules()
    {
        if (!BuildVulkan)
        {
            ModuleExcludeList.Add("VulkanRHI");
        }
        if (!BuildCSharp)
        {
            ModuleExcludeList.Add("CSharpContainer");
            ModuleExcludeList.Add("CSharpCore");
            ModuleExcludeList.Add("CSharpBridge");
            ModuleExcludeList.Add("CSTestGame");
            GlobalDefines.Add("NOCSHARP");
        }
        if (BuildPhysx)
        {
            GlobalDefines.Add("USE_PHYSX");
            ModuleExcludeList.Add("TDPhysics");
        }
        WindowTenVersionTarget = "1903";
        // WindowTenVersionTarget = "1803";
    }
    public override ModuleDef GetCoreModule()
    {
        ModuleDef CoreModule = new ModuleDef(this);
        CoreModule.ModuleName = "Core";
        CoreModule.ModuleOutputType = ModuleDef.ModuleType.DLL;
        CoreModule.SolutionFolderPath = "Engine/Core";
        CoreModule.SourceFileSearchDir = "Core";
        CoreModule.PCH = "Stdafx";
        CoreModule.IncludeDirectories.Add("/source/ThirdParty");
        CoreModule.IncludeDirectories.Add("/source/Core");
        CoreModule.IncludeDirectories.Add("/source/ThirdParty/freetype2");
        CoreModule.IncludeDirectories.Add("/source/TDPhysics");
        CoreModule.UseCorePCH = false;

        CoreModule.UseUnity = true;
        CoreModule.OutputObjectName = "Core";

        CoreModule.IsCoreModule = true; 
        CoreModule.SystemLibNames.Add("winmm.lib");
        CoreModule.SystemLibNames.Add("shlwapi.lib");
        CoreModule.SystemLibNames.Add("dbghelp.lib");
        CoreModule.StaticLibraries.Add(new LibDependency("Xinput9_1_0.lib", "win64"));
        CoreModule.ExcludedFolders.Add("**/ThirdParty/Audio/**");
        CoreModule.UnityBuildExcludedFolders.Add("\\Core\\Platform\\Extra\\");
       // CoreModule.ExcludedFoldersNew.Add(new FolderPlatformPair("**/Windows/**", "!win64"));
        string[] ThirdPartyModules = new string[] {
            "freetypeBuild",
            "assimpBuild",
           "nvapiBuild", 
           // "WWiseBuild",
            "OpenVRBuild",
            //"AfterMathBuild",
            "PixBuild"
            };
        CoreModule.ThirdPartyModules.AddRange(ThirdPartyModules);

        if (BuildPhysx)
        {
            CoreModule.ThirdPartyModules.Add("PhysxBuild");
        }
        else
        {
            CoreModule.ModuleDepends.Add("TDPhysics");
        }
        if (BuildVulkan)
        {
            CoreModule.PreProcessorDefines.Add("WITH_VK");
        }
        //CoreModule.GameModuleName = "BleedOutGame";
        CoreModule.GameModuleName = "TestGame";
        return CoreModule;
    }
}