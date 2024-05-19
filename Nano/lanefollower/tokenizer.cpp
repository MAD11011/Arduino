#include <Arduino.h>
#include "tokenizer.h"

serial_types CheckType(char* str);

tokens Tokenize(char* str){
    char last = ' ';
    token tokens[32] ;
    uint8_t num_tokens = 0;

    while(*str != '\0' ){
        if(*str != ' ' && last == ' '){
           tokens[num_tokens].type = STRING;
           tokens[num_tokens].str  = str;
           num_tokens++;
           last = *str;

        }
        if(*str == ' '){
            *str = '\0';
            //tokens[num_tokens].type = CheckType(tokens[num_tokens].str);
            last = ' ' ;
        }
        str++;
    };
    struct tokens res;
    res.num_tokens = num_tokens;
    memcpy(res.tokens,tokens,sizeof(res.tokens));
    return res;
    
}

serial_types CheckType(char* str){

    const char numerical_notation[] = "xXbBoO";
    char notation = '\0';
    char c = 0;
    char *str_p = str;
    bool contains_period = false;
    for(;*str_p != '\0';str_p++){
        if(notation == '\0'){
            if(!isdigit(*str_p)){
                if(*str_p == '.'){
                    contains_period = true;
                };
                if(str_p == str+1 && *str == '0'){
                    const char *n = numerical_notation;
                    for(;*n != '\0';n++){
                        if(*str_p == *n){
                            notation = *n;
                            break;
                        }
                    }
                    if(*n == '\0'){
                        return STRING;
                    }
                }else{
                    return STRING;
                }
            }
        }else{
            if(!isxdigit(*str_p)){
                return STRING;
            }
        }
    }
    if(contains_period){
        return FLOAT;
    }
    return INT;



}
