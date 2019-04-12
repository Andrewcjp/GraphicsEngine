
using EngineBuildTool;

class CoreTargetRules : TargetRules
{
    bool BuildPhysx = true;
    bool BuildVulkan = false;
    public CoreTargetRules()
    {
        if (!BuildVulkan)
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
        CoreModule.IncludeDirectories.Add("/source/ThirdParty");
        CoreModule.IncludeDirectories.Add("/source/Core");
        CoreModule.IncludeDirectories.Add("/source/ThirdParty/freetype2");
        CoreModule.IncludeDirectories.Add("/source/TDPhysics");
        CoreModule.UseCorePCH = false;
        CoreModule.ModuleDepends.Add("TDPhysics");
        CoreModule.UseUnity = true;
        CoreModule.OutputObjectName = "BleedOut";
        CoreModule.IsCoreModule = true;
        string[] ThirdPartyModules = new string[] {
            "freetypeBuild",
            "assimpBuild",
            "nvapiBuild",
            "WWiseBuild"
        };
        CoreModule.ThirdPartyModules.AddRange(ThirdPartyModules);
        if (BuildPhysx)
        {
            CoreModule.ThirdPartyModules.Add("PhysxBuild");
        }
        if (BuildVulkan)
        {
            CoreModule.PreProcessorDefines.Add("WITH_VK");
        }
        return CoreModule;
    }
}