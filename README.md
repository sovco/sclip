# sclip
Simple  CLI Parser

    -a | --ass            <string>  Description of this option adsadasdasdasdasdadasda
    -b                    <string>  Description of this option
         --longer-option  <string>  Description of this option
         --longer-optn    <bool>    Description of this option
                                    multiline and shits


Short option is always len 2 bytes
Long opt can be of any lenght find the lenght of Long-opt

1) Get the longest case before the description
    1 symbol '"'
    so tab 4 spaces
    2 symbols for short opt
    3 symbols between short opt and long opt if both options are present 2 symbol is '|'
    n amount of symbols for long opt
    2 spaces before type
    n + 2 lenght for type and symbols '<' and '>'
    2 spaces after
    50 would be the description line
2) Split the desc somehow
    jump to symbol at 50
    if space go to new line
    if alpha back up to space
