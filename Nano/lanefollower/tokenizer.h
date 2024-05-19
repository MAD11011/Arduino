const char null_str = '\0';
enum serial_types{
    INT,FLOAT,STRING
};

struct token{
    serial_types type ;
    const char* str = &null_str;
};

struct tokens{
    uint8_t num_tokens;
    token tokens[32];
};



tokens Tokenize(char* str);

     
