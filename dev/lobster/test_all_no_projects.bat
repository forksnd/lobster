@rem @echo off
@cls
@start /B ..\..\bin\lobster.exe --non-interactive-test --silent ..\..\tests\unittest.lobster
@for /R ..\.. %%F in (modules\*.lobster) do @(@start /B ..\..\bin\lobster.exe --non-interactive-test --silent %%F)
@for /R ..\.. %%F in (tests\*.lobster) do @(@start /B ..\..\bin\lobster.exe --non-interactive-test --silent %%F)
@for /R ..\.. %%F in (samples\*.lobster) do @(@start /B ..\..\bin\lobster.exe --non-interactive-test --silent %%F)
@pause
