workspace "RETSpoof"
	location "VStudio"
	configurations { "Release", "Debug" }
	platforms { "X64" }
	startproject "RETSpoof"
	pic "On"
	systemversion "10.0.17763.0"
	characterset "ASCII"

project "RETSpoof"
	kind "ConsoleApp"
	language "C++"
	targetdir "Binary"
	includedirs { "Source/Detours" }

	files { "Source/**" }
 
	vpaths {
		["headers"] = { "**.h", "**.hxx", "**.hpp" }
	}

	filter "configurations:Release"
		defines { "DETOURS_X64", "DETOURS_64BIT", "NDEBUG" }
		optimize "On"

	filter "configurations:Debug"
		defines { "DETOURS_X64", "DETOURS_64BIT", "NDEBUG" }
		symbols "On"

-- Cleanup
if _ACTION == "clean" then
	os.rmdir("Binary");
	os.rmdir("VStudio");
end
