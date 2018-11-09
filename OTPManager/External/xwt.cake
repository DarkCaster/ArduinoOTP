var target = Argument("target", "Default");
var configuration = Argument("configuration", "Release");

private void Patch(string workDir, string patch, int p)
{
  var settings=new ProcessSettings()
  {
    WorkingDirectory = new DirectoryPath(workDir),
    Arguments = new ProcessArgumentBuilder().Append("-p"+p.ToString()).Append("-i").Append(patch),
  };
  if(StartProcess("patch",settings)!=0)
    throw new Exception("patch failed!");
}

private bool SNTest(string assembly)
{
  var settings=new ProcessSettings()
  {
    WorkingDirectory = new DirectoryPath("."),
    Arguments = new ProcessArgumentBuilder().Append("-v").Append(assembly),
  };
  if(StartProcess("sn",settings)==1)
    return true;
  return false;
}

private void SNSign(string assembly, string key)
{
  var settings=new ProcessSettings()
  {
    WorkingDirectory = new DirectoryPath("."),
    Arguments = new ProcessArgumentBuilder().Append("-R").Append(assembly).Append(key),
  };
  if(StartProcess("sn",settings)!=0)
    throw new Exception("full-sign failed!");
}

Task("Patch").Does(() =>
{
  Patch("xwt","../xwt.nuget.patch",1);
  Patch("xwt","../xwt.wpf.patch",1);
});

Task("Build").IsDependentOn("Patch").Does(() =>
{
  MSBuild("xwt/Xwt.sln", settings => {
    settings.SetConfiguration(configuration).WithTarget("Xwt:Rebuild;Xwt_Gtk3:Rebuild;Xwt_WPF:Rebuild");
    if(IsRunningOnUnix())
    settings.ToolPath = "/usr/bin/msbuild"; });

  if(SNTest("xwt/Xwt/bin/Release/Xwt.dll"))
    SNSign("xwt/Xwt/bin/Release/Xwt.dll","xwt/xwt.snk");

  if(SNTest("xwt/Xwt.Gtk/bin/Release/Xwt.Gtk3.dll"))
    SNSign("xwt/Xwt.Gtk/bin/Release/Xwt.Gtk3.dll","xwt/xwt.snk");

  if(SNTest("xwt/Xwt.WPF/bin/Release/Xwt.WPF.dll"))
    SNSign("xwt/Xwt.WPF/bin/Release/Xwt.WPF.dll","xwt/xwt.snk");
});

private void PackFrom(string path, string nuspec, params string[] extraCopy)
{
  try { DeleteDirectory("output", recursive:true); }
  catch { }
  CreateDirectory("output");
  if(extraCopy!=null)
    foreach(var extra in extraCopy)
      CopyFiles(extra,"output");
  CreateDirectory("output/net40");
  CopyFiles(path,"output/net40");
  var nuGetPackSettings = new NuGetPackSettings { BasePath = "output", };
  NuGetPack(nuspec, nuGetPackSettings);
}

Task("Pack").IsDependentOn("Build").Does(() =>
{
  PackFrom("xwt/Xwt/bin/Release/*","xwt/Xwt/Xwt.nuspec");
  PackFrom("xwt/Xwt.Gtk/bin/Release/*","xwt/Xwt.Gtk/Xwt.Gtk3.nuspec","xwt/Xwt.Gtk/Xwt.Gtk3.dll.config","xwt/Xwt.Gtk/Xwt.Gtk3.nuget.props");
  PackFrom("xwt/Xwt.WPF/bin/Release/*","xwt/Xwt.WPF/Xwt.WPF.nuspec");
});

Task("Default").IsDependentOn("Pack");
RunTarget(target);
