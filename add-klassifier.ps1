Write-Output "Vers�o do powershell: "

$PSVersionTable.PSVersion

$registryPath = "HKCU:\Software\Classes\Directory\Background\shell\Klassifier\command"

$Name = "command"

Write-Output $values

if(!(Test-Path $registryPath)) {
    Write-Output "Registro n�o encontrado, realizando a cria��o"
    New-Item -Path $registryPath -Value "C:\alias\klassifier.exe" -Force | Out-Null 
}

Write-Output "Adicionado" 
