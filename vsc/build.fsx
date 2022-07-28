#if FAKE

#r "paket:
nuget Fake.DotNet.Cli
nuget Fake.DotNet.MSBuild
nuget Fake.IO.FileSystem
nuget Fake.Core.SemVer
nuget Fake.Core.Environment
nuget Fake.Core.Target //"

#endif

#load "./.fake/build.fsx/intellisense.fsx"
open Fake.Core
open Fake.IO
open Fake.IO.Globbing.Operators

let buildDir = "./build/"
let buildMode = Environment.environVarOrDefault "buildMode" "Release"

// *** Define Targets ***
Target.create "Clean" (fun _ ->
  Trace.log " --- Cleaning stuff --- "
  Shell.cleanDir buildDir
)

Target.create "BuildNative" (fun _ ->
  Trace.log " --- Build native libraries --- "
)

Target.create "Build" (fun _ ->
  Trace.log " --- Building the app --- "
)

Target.create "Deploy" (fun _ ->
  Trace.log " --- Deploying app --- "
)

open Fake.Core.TargetOperators

// *** Define Dependencies ***
"Clean"
  ==> "BuildNative"
  ==> "Build"
  ==> "Deploy"

// *** Start Build ***
Target.runOrDefault "Deploy"
