workspace "A5_s21_memory"
  language "C++"
  cppdialect "C++17"

  objdir "build"
  targetdir "bin"

  configurations { "debug", "release", "asan", "test" }

  location "src"

  filter "release"

  filter "release"
    defines { "NDEBUG" }
    optimize "On"

  filter "debug"
    symbols "On"
    defines { "DEBUG" }

  filter "asan"
    symbols "On"
    buildoptions { "-fsanitize=address" }
    linkoptions { "-fsanitize=address" }

  filter "tags:untestable"
    removeconfigurations "test"

  filter "tags:testable"
    configurations { "test" }

  filter "tags:test"
    removeconfigurations "*"
    configurations { "test" }

  filter { "test", "tags:collect-coverage" }
    buildoptions { "-fprofile-arcs", "-ftest-coverage" }
    linkoptions { "--coverage" }
    links { "gcov" }

  filter { "test", "tags:use-coverage" }
    linkoptions { "--coverage" }
    links { "gcov" }

  project "cli"
    kind "ConsoleApp"

    tags { "untestable" }

    location "%{wks.location}/cli"

    files { "%{prj.location}/src/**.cpp" }
    includedirs { "%{prj.location}/include", "%{wks.location}/s21_memory/include" }

    links { "s21_memory" }

  project "s21_memory"
    kind "StaticLib"

    tags { "testable", "collect-coverage" }

    location "%{wks.location}/s21_memory"

    files { "%{prj.location}/src/**.cpp" }
    includedirs { "%{prj.location}/include" }

  project "s21_memory_test"
    kind "ConsoleApp"

    tags { "test", "use-coverage" }

    location "%{wks.location}/s21_memory_test"

    files { "%{wks.location}/s21_memory_test/**.cpp" }
    includedirs { "%{wks.location}/s21_memory/include" }

    links { "gtest", "gmock" }
    links { "s21_memory" }

    postbuildcommands {
      "%{cfg.buildtarget.abspath}",
      "lcov -c -d %{cfg.objdir}/.. -o %{cfg.buildtarget.directory}/%{cfg.buildtarget.basename}_coverage.info",
      "genhtml %{cfg.buildtarget.directory}/%{cfg.buildtarget.basename}_coverage.info -o %{cfg.buildtarget.directory}/%{cfg.buildtarget.basename}_coverage_report"
    }

