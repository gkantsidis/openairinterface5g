namespace OpenAir.Bench

open System

[<Serializable>]
type BenchException  =
    class
        inherit Exception

        new () = { inherit Exception() }
        new (message : string) = { inherit Exception(message) }
        new (message : string, inner : Exception) = { inherit Exception(message, inner) }

    end
