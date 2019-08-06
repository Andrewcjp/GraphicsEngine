using EngineBuildTool;

class EditorModule : ModuleDef
{
    public EditorModule()
    {
        SolutionFolderPath = "Engine/Editor";
        ModuleName = "Editor";
        SourceFileSearchDir = "Editor";
        PCH = "EditorPCH";
        ModuleOutputType = ModuleDef.ModuleType.DLL;
        UseCorePCH = false;
        UseUnity = true;
    }
}