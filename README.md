# INF01151

## Building

At the root of the project, in a unix terminal run:

`make`

### Server only

`make server`

### Client only

`make client`

## Running

### Server

`make runser`

Or, after building:

`./server`

### Client

`make runcli1`
`make runcli2`
`make runcli3`

## Cleaning

`make clean`

## Tarefas Pendentes

- [x] Um perfil é uma cadeia de 4 a 20 caracteres (Atualmente é de 1 a 20) (isValidUsername);
- [x] Implementar commando SEND no servidor;
- [ ] Um usuário pode ter até duas sessões simultâneas (servidor recusa novas tentativas) (Quando terminar uma sessão, a outra não deve ser afetada);
- [x] Receber todas as notificações no período que esteve offline (caso tenha outra sessão com mesmo nome considera como recebido);
- [x] Guardar lista de notificações enviadas peloo usuário;
- [x] Guardar fila de notificações pendentes para o usuário;
- [x] Adicionar timestamp da notificação;
- [x] Número de usuários que ainda não receberam uma notificação (Dentro da estrutura da notificação);
- [x] Implementação produtor/consumidor no servidor (recebimento e entrega de mensagens para clientes);
- [x] Tratar CTRL+C (interrupção) ou CTRL+D (fim de arquivo) (Persistir dados e sinalizar o servidor que o usuário está desconectando)
- [ ] Persistência de dados em caso de falha dos servidor (usuários e relação de seguidores)
- [ ] Formatar notificação `@user "message" (timestamp)`
- [x] Mensagem deve aceitar espaços (branch spaceCharacters)

## Por último

- [ ] Servidor deve ter um produtor e consumidor por cliente;
- [ ] Adicionar quantidade de caracteres da notificação;
- [ ] Lidar com perda de pacotes;
