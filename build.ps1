Remove-Item -Force -Recurse "build/"
Copy-Item -Force -Recurse "./data" "build/data"
Copy-Item -Force "../build-CForumServer-Desktop_Qt_5_11_2_MSVC2017_64bit-Release/release/*.exe" "build/"
Copy-Item -Force "../build-CForumClient-Desktop_Qt_5_11_2_MSVC2017_64bit-Release/release/*.exe" "build/"
Get-ChildItem -Path "build/*.exe" | ForEach-Object {
    windeployqt --qmldir "./qml" $_.FullName
}
