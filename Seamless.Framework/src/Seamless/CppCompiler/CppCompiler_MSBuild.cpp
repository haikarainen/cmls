
#include "Seamless/CppCompiler/CppCompiler_MSBuild.hpp"

#include "Seamless/Filesystem.hpp"
#include "Seamless/Async.hpp"
#include "Seamless/Error.hpp"

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <ShlObj.h>

#include <map>

bool cmls::CppCompiler_MSBuild::compile(CppProject const& project)
{
  std::map<cmls::CppTargetType, std::string> targetToExt = {{TT_Executable, ".exe"}, {TT_DynamicLibrary, ".dll"}};


  auto intermediatePath = cmls::Directory(project.intermediatePath).fullPath();
  auto targetPath = cmls::Directory(project.targetPath).fullPath();

  std::string projectFilename = intermediatePath + "\\" + project.targetName + ".vcxproj";
  std::string outputFilename = targetPath + "\\" + project.targetName + targetToExt.at(project.targetType);

  PWSTR programFiles = NULL;
  if (SHGetKnownFolderPath(FOLDERID_ProgramFilesX86, 0, nullptr, (PWSTR*)&programFiles) != S_OK)
  {
    LogError("Failed to find program files x86 folder");
    writeOutput("Failed to find program files x86 folder");
    return false;
  }

  auto progFiles = cmls::utf16to8(programFiles);

  CoTaskMemFree(programFiles);

  // find msbuild
  auto finderCmd = "\"" + progFiles + "\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\\**\\Bin\\MSBuild.exe";
  LogNotice("Command: %s", progFiles.c_str());
  writeOutput(cmls::formatString("Command: %s",progFiles.c_str()));
  auto finderProcess = cmls::AsyncProcess::create(finderCmd);
  finderProcess->runSynchronously();

  auto msBuildPath = cmls::trim(finderProcess->output());
  if (!cmls::File(msBuildPath).exist())
  {
    writeOutput(cmls::formatString("Failed to find MSBuild path, vswhere.exe claims \"%s\"", msBuildPath.c_str()));
    LogError("Failed to find MSBuild path, vswhere.exe claims \"%s\"", msBuildPath.c_str());
    return false;
  }

  // generate a project for the view
  // @todo, use cmls compiler here? lol

  std::vector<std::string> libraryPaths = {};
  std::vector<std::string> includePaths = {};
  std::vector<std::string> defines = {};
  std::vector<std::string> libraries = {};

  for (auto m : project.modules)
  {
    auto mlp = m.linkerPaths();
    libraryPaths.insert(libraryPaths.begin(), mlp.begin(), mlp.end());

    auto mip = m.includePaths();
    includePaths.insert(includePaths.begin(), mip.begin(), mip.end());

    auto md = m.defines();
    defines.insert(defines.begin(), md.begin(), md.end());

    auto ml = m.links();
    libraries.insert(libraries.begin(), ml.begin(), ml.end());
  }

  std::stringstream data;
  data << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  data << "<Project DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
  data << "  <ItemGroup>\n";
  data << "    <ProjectConfiguration Include=\"Release|x64\">\n";
  data << "      <Configuration>Release</Configuration>\n";
  data << "      <Platform>x64</Platform>\n";
  data << "    </ProjectConfiguration>\n";
  data << "  </ItemGroup>\n";
  data << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.default.props\" />\n";
  data << "  <PropertyGroup>\n";
  data << "    <TargetName>" << project.targetName << "</TargetName>\n";
  data << "    <OutDir>" << targetPath << "\\</OutDir>\n";
  data << "    <IntDir>" << intermediatePath << "\\</IntDir>\n";
  data << "    <ConfigurationType>DynamicLibrary</ConfigurationType>\n";
  data << "    <PlatformToolset>v142</PlatformToolset>\n";
  data << "  </PropertyGroup>\n";
  data << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n";
  data << "  <PropertyGroup>\n";
  data << "    <IncludePath>" << cmls::join(includePaths, ";") << ";$(VC_IncludePath);$(WindowsSDK_IncludePath);</IncludePath>\n";
  data << "    <LibraryPath>" << cmls::join(libraryPaths, ";") << ";$(VC_LibraryPath_x64);$(WindowsSDK_LibraryPath_x64);$(NETFXKitsDir)Lib\\um\\x64</LibraryPath>\n";
  data << "  </PropertyGroup>\n";
  data << "  <ItemDefinitionGroup>\n";
  data << "    <ClCompile>\n";
  data << "      <WarningLevel>Level3</WarningLevel>\n";
  data << "      <Optimization>MaxSpeed</Optimization>\n";
  data << "      <SDLCheck>true</SDLCheck>\n";
  data << "      <ConformanceMode>true</ConformanceMode>\n";
  data << "      <MultiProcessorCompilation>true</MultiProcessorCompilation>\n";
  data << "      <LanguageStandard>stdcpplatest</LanguageStandard>\n";
  data << "      <PreprocessorDefinitions>" << cmls::join(defines, ";") << ";_WINDLL;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
  data << "      <DisableSpecificWarnings>4251</DisableSpecificWarnings>\n";
  data << "    </ClCompile>\n";
  data << "    <Link>\n";
  data << "      <LinkIncremental>false</LinkIncremental>\n";
  data << "      <AdditionalDependencies>" << cmls::join(libraries, ";") << ";ws2_32.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)</AdditionalDependencies>\n";
  data << "    </Link>\n";
  data << "  </ItemDefinitionGroup>\n";
  data << "  <ItemGroup>\n";
  for(auto src : project.sources)
  {
    data << "    <ClCompile Include=\"" << src << "\" />\n";
  }
  data << "  </ItemGroup>\n";
  data << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Targets\" />\n";
  data << "</Project>\n";

  // write the project to disk
  if (!cmls::File(projectFilename).writeString(data.str()))
  {
    writeOutput(cmls::formatString("Failed to write project file %s", projectFilename.c_str()));
    LogError("Failed to write project file %s", projectFilename.c_str());
    return false;
  }

  // build the generated project
  auto buildCmd = cmls::join({msBuildPath, projectFilename, "/property:VCBuildAdditionalOptions=/useenv /property:Configuration=Release /property:Platform=x64 /verbosity:minimal /nologo" }, " ");
  auto buildProcess = cmls::AsyncProcess::create(buildCmd);
  buildProcess->runSynchronously();
  if (buildProcess->returnCode() != 0)
  {
    LogError("MSBuild failed");
    writeOutput("MSBuild failed, see build log below");
    writeOutput("===================================");
    for (auto l : cmls::split(buildProcess->output(), { '\n' }))
    {
      writeOutput(l);
    }
    writeOutput("===================================");
    writeOutput("");

    return false;
  }

  // verify output file exists
  if (!cmls::File(outputFilename).exist())
  {
    writeOutput("Build reported as OK but expected output file does not exist");
    LogError("Build reported as OK but expected output file does not exist");
    return false;
  }

  writeOutput(cmls::formatString("Built target %s", project.targetName));
  LogNotice("Built target %s", project.targetName);

  return true;
}
