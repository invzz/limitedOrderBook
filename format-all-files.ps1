# Get the path to the latest 'ms-vscode.cpptools' extension

try {
        $extensionPath = Get-ChildItem -Directory -Path "${env:USERPROFILE}\.vscode\extensions" `
        | Where-Object { $_.Name -like "ms-vscode.cpptools-*-*" } `
        | Sort-Object LastWriteTime -Descending `
        | Select-Object -First 1 -ExpandProperty FullName

        # Check if clang-format.exe exists
        $clangFormatPath = "${extensionPath}\LLVM\bin\clang-format.exe"
        $clangTidyPath = "${extensionPath}\LLVM\bin\clang-tidy.exe"
        if (Test-Path $clangFormatPath) {
                Get-ChildItem -Recurse -Include *.cc, *.hh | Where-Object { $_.FullName -notmatch "\\build\\" } 
                | ForEach-Object { 
                        Write-Host "Formatting file: $($_.FullName)"
                        & "$clangFormatPath" -i $_.FullName
                }
        }
        else {
                Write-Host "clang-format executable not found!"
        }
}
catch {
        Write-Host "An error occurred: $_"
        Write-Host "Please ensure that the 'ms-vscode.cpptools' extension is installed and up to date."
}