# MSQLIGHTS

msqlights is a dmx client with a peculiar approach

## Server Communication

You can modify any modifiable in the engine using OSC on port 9000.
The format is the following
```
/msqlights/"modifiable id"/"parameter name"/"value"
```

Otherwise you can setup a connection with socket on port 8888
Send and recieve strings separated by /n

echoes the same command followed by :OK on success
echoes the same command followed by :FAIL on fail 

```
//set "modifiable id" "parameter name" "value"
set 0xDEADBEEF position.x 2
// would return set 0xDEADBEEF position.x 2 :OK

//create "modifiable type" "name"
create Fixture Bak3
// would return createFixture Bak3 :OK 0xF00DBABE

//get "modifiable id" "parameter name"
get 0xDEADBEEF position.x
// would return set 0xDEADBEEF position.x :OK f 2

//add "modifiable id" "fixture id"
add 0xDEADBEEF 0xF00DBABE
// would return add 0xDEADBEEF 0xF00DBABE :OK

//getids
getids
// would return getids :OK 0xDEADBEEF 0xF00DBABE

//getparams "modifiable id"
getparams 0xDEADBEEF
// would return getparams 0xDEADBEEF :OK position.x position.y ...

//getid
getid Bak3
// would return getid Bak3 :OK 0xF00DBABE ...

```
