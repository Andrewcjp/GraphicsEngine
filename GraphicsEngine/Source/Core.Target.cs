using EngineBuildTool;

class CoreTargetRules : TargetRules
{
    bool BuildPhysx = true;
    bool BuildVulkan = false;
    public CoreTargetRules()
    {
        LibSearchPaths.Add(new LibSearchPath("\\64", LibBuildConfig.General));
        LibSearchPaths.Add(new LibSearchPath("\\64\\Debug", LibBuildConfig.Debug));
        LibSearchPaths.Add(new LibSearchPath("\\64\\Release", LibBuildConfig.Optimized));
        if (BuildPhysx)
        {
            LibSearchPaths.Add(new LibSearchPath("\\Physx\\Release", LibBuildConfig.Optimized));
            LibSearchPaths.Add(new LibSearchPath("\\Physx\\Debug", LibBuildConfig.Debug));
            LibSearchPaths.Add(new LibSearchPath("\\Physx", LibBuildConfig.General, true));
        }
        LibSearchPaths.Add(new LibSearchPath("\\Core", LibBuildConfig.General, true));
        if (BuildVulkan)
        {
            LibSearchPaths.Add(new LibSearchPath("\\Vulkan", LibBuildConfig.General, true));
        }
        else
        {
            ModuleExcludeList.Add("VulkanRHI");
        }
        if (BuildPhysx)
        {
            GlobalDefines.Add("USE_PHYSX");
        }
    }
    public override ModuleDef GetCoreModule()
    {
        ModuleDef CoreModule = new ModuleDef();
        CoreModule.ModuleName = "Core";
        CoreModule.ModuleOutputType = ModuleDef.ModuleType.EXE;
        CoreModule.SolutionFolderPath = "Engine/Core";
        CoreModule.SourceFileSearchDir = "Core";
        CoreModule.PCH = "Stdafx";
        CoreModule.IncludeDirectories.Add("/Include");
        CoreModule.IncludeDirectories.Add("");
        CoreModule.IncludeDirectories.Add("/Include/freetype2");
        CoreModule.UseCorePCH = false;
        return CoreModule;
    }
}