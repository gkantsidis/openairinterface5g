#r "paket:
nuget Fake.IO.FileSystem
nuget Fake.DotNet.MSBuild
nuget Fake.Core.Target //"
#load "./.fake/build.fsx/intellisense.fsx"

open Fake.Core
open Fake.IO
open Fake.IO.Globbing.Operators
open Fake.DotNet

let buildDir = "./build/"

let coreProjects = [
    "./OpenAir/LDPC/LDPC.vcxproj"
    "./OpenAir/OpenAirDll/OpenAirDll.vcxproj"
]

Target.create "Clean" (fun _ ->
    Trace.log "--- Cleaning output ---"
    Shell.cleanDir buildDir
)

Target.create "BuildCore" (fun _ ->
    Trace.log " --- Building Core Projects ---"
    coreProjects
    |> MSBuild.runRelease id buildDir "Build"
    |> Trace.logItems "AppBuild-Output: "
)

Target.create "Build" (fun _ ->
    Trace.log "--- Building All Projects ---"
)

Target.create "Deploy" (fun _ ->
    Trace.log "--- Create deployment packages ---"
)

open Fake.Core.TargetOperators

"Clean"
    ==> "BuildCore"
    ==> "Build"
    ==> "Deploy"

Target.runOrDefault "Build"
