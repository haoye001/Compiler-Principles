$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    bison -d -o lrparser.tab.c lrparser.y
    if ($LASTEXITCODE) { throw 'Bison failed' }
    flex -o lrlex.yy.c lrlex.l
    if ($LASTEXITCODE) { throw 'Flex failed' }
    gcc -std=c11 -Wall -Wextra -Wno-unused-function main.c codegen.c ast.c lrparser.tab.c lrlex.yy.c -o codegen.exe
    if ($LASTEXITCODE) { throw 'GCC failed' }
} finally {
    Pop-Location
}
