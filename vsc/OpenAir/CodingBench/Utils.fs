namespace OpenAir.Bench

[<AutoOpen>]
module Utils =
    open System

    let inline entropy p = -p * Math.Log(p, 2.0) - (1.0 - p) * Math.Log(1.0 - p, 2.0)

