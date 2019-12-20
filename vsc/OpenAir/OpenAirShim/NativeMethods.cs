
namespace OpenAirShim
{
    using System;
    using System.IO;
    using System.Reflection;
    using System.Runtime.InteropServices;

    internal static class SafeNativeMethods
    {
        [DllImport("OpenAirDll.dll", EntryPoint = "ldpc_encode_simple", CallingConvention =CallingConvention.Cdecl)]
        internal static extern int Encode(IntPtr input, int input_length, IntPtr encoded, int base_graph);

        static SafeNativeMethods()
        {
            InitializeRuntime.CheckAndLoad();
        }
    }

    internal static class InitializeRuntime
    {
        [DllImport("kernel32.dll")]
        private static extern IntPtr LoadLibrary(string dllToLoad);

        public static void CheckAndLoad()
        {
            if (_is_initialized)
            {
                return;
            }

            lock (_lock)
            {
                if (_is_initialized)
                {
                    return;
                }

                try
                {
                    var thisAssembly = Assembly.GetAssembly(typeof(SafeNativeMethods));
                    var assembly_info = new FileInfo(thisAssembly.Location);
                    var native_library = Path.Combine(assembly_info.DirectoryName, "native", _sub_directory, "OpenAirDll.dll");
                    if (File.Exists(native_library) == false)
                    {
                        throw new OpenAirShimException($"Cannot find native library in {native_library}");
                    }

                    var native_assembly = LoadLibrary(native_library);
                    if (native_assembly == default(IntPtr))
                    {
                        throw new OpenAirShimException("Cannot load native library");
                    }

                    _is_initialized = true;
                }
                catch (Exception ex)
                {
                    throw new OpenAirShimException("Cannot load native library", ex);
                }
            }
        }

        private static bool _is_initialized = false;
        private static object _lock = new object();
        private static string _sub_directory = Environment.Is64BitProcess ? "x64" : "win32";
    }

    public static class LdpcConstants
    {
        public const int PacketSize = 1056;
        public const int BufferSize = 68 * 384;
    }
}
