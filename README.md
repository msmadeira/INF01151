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

- [ ] Um perfil é uma cadeia de 4 a 20 caracteres (Atualmente é de 1 a 20) (isValidUsername);
- [ ] Um usuário pode ter até duas sessões simultâneas (servidor recusa novas tentativas) (Quando terminar uma sessão, a outra não deve ser afetada);
- [ ] Receber todas as notificações no período que esteve offline (caso tenha outra sessão com mesmo nome considera como recebido);
- [ ] Guardar lista de notificações enviadas para o usuário;
- [ ] Guardar fila de notificações pendentes para o usuário;
- [ ] Adicionar timestamp da notificação;
- [ ] Número de usuários que ainda não receberam uma notificação (Dentro da estrutura da notificação);
- [ ] Implementação produtor/consumidor no servidor (recebimento e entrega de mensagens para clientes);
- [ ] Tratar CTRL+C (interrupção) ou CTRL+D (fim de arquivo) (Persistir dados e sinalizar o servidor que o usuário está desconectando)
- [ ] Persistência de dados em caso de falha dos servidor (usuários e relação de seguidores)
- [ ] Formatar notificação `@user "message" (timestamp)`
- [ ] Mensagem deve ter até 128 caracteres e aceitar espaços

## Por último
- [ ] Adicionar quantidade de caracteres da notificação;
- [ ] Lidar com perda de pacotes;
