# Disciplina de Redes de Computadores - SSC0142, ICMC-USP

## Membros

- Gildo Gutemberg Alves Oliveira - 11832732
- Otto Cruz Fernandes - Otto Cruz Fernandes
- Sofhia de Souza Gonçalves - 11735035

## Sistema Operacional

- Linux Ubuntu 20.04

## Compilador

- g++ 9.4.0

## Instruções

Comandos do make file:

- make compile_all: compila todos os arquivos, gerando executáveis para o cliente e para o servidor
- make compile_client: compila os arquivos relacionados ao cliente
- make compile_server: compila os arquivos relacionados ao servidor
- make run_server: executa o servidor
- make deploy_server: compila e executa o servidor
- make clear_client: apaga o executável do cliente
- make clear_server: apaga o executável do servidor

Para utilizar o sistema deve-se executar primeiro o servidor. Depois o cliente, com o comando ./client ip_address, substituindo ip_address pelo IP do servidor ao qual se deseja conectar.
