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

- [ ] Replicação Passiva
  - [ ] Implementar replicação com 1 Replica Manager e múltiplas instâncias de back-up
  - [ ] Adicionar FrontEnd entre a comunicação do cliente e o conjunto dos RMs (manter transparência é um requisito), deve ter 1 FrontEnd por sessão (e.g se um usuário tem duas sessões de uso, teremos dois FE's)
  - [ ] Garantir que todos os clientes utilizarão a mesma cópia primária (RM primário)
  - [ ] Após cada operação, o RM primário irá propagar o estado aos RM's de backup (usuários, seguidores, notificações etc)
  - [ ] Somente após os backups serem atualizados o primário confirmará a operação ao cliente.
- [ ] Eleição de Líder
  - [ ] Implementar um algoritmo de eleição de lider para os múltiplos servidores (Algoritmo do Anel ou Algoritmo do Valentão)
  - [ ] Quando o servidor principal falhar, o algoritmo de eleição de líder deverá ser utilizado para determinar o próximo servidor primário (um dos backups assumirá, mantendo um estado consistente do sistema)
  - [ ] Atualizar as informações sobre o novo líder nos FE dos clientes
