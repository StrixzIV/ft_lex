__HEADER_PLACEHOLDER__

class Lexer:
    """
    A generated lexer class using Deterministic Finite Automata (DFA).
    The transition and acceptance tables are injected during generation.
    """
    
__TABLES_PLACEHOLDER__

    def __init__(self, input_stream=sys.stdin):
        """
        Initializes the lexer.
        :param input_stream: The file-like object to read from (defaults to stdin).
        """
        self.input_stream = input_stream
        self.yytext = ""
        self.yyleng = 0
        self.pushback_buffer = []  # Stack for unreading characters
        self._yy_more_flag = False
        self._yy_more_len = 0
        
        # Start conditions
        self.INITIAL = 0
        self.yy_start = 0  # Current start condition
    
    def BEGIN(self, state):
        """Switch to a different start condition."""
        self.yy_start = state

    def read_char(self):
        """Reads one character from the input stream."""
        if self.pushback_buffer:
            return self.pushback_buffer.pop()
        char = self.input_stream.read(1)
        return char if char else None

    def unread_char(self, char):
        """Puts back one character for later reading."""
        if char is not None:
            self.pushback_buffer.append(char)

    # --- POSIX Scanner Functions ---
    
    def input(self):
        """Read next character from input. Returns empty string on EOF."""
        c = self.read_char()
        return c if c else ''
    
    def unput(self, c):
        """Push character back onto input stream."""
        if c:
            self.unread_char(c)
    
    def yymore(self):
        """Set flag to append next token to current yytext."""
        self._yy_more_flag = True
    
    def yyless(self, n):
        """Return all but first n characters to input stream."""
        # Push back excess characters in reverse order
        for i in range(len(self.yytext) - 1, n - 1, -1):
            self.unread_char(self.yytext[i])
        self.yytext = self.yytext[:n]
        self.yyleng = n
    
    def REJECT(self):
        """Try next best matching rule.
        NOTE: Full REJECT requires generator-level support.
        This sets a flag that the yylex driver should check."""
        self._yy_reject_flag = True

    def yylex(self):
        """
        The main lexer driver function.
        It runs the DFA and returns a matched token ID or 0 for EOF.
        """
        
        while True:
            current_state = 0
            last_accepting_state = -1
            
            buffer = []
            last_accepting_idx = -1
            
            # Read first char
            c = self.read_char()
            if c is None:
                return 0  # EOF
            
            buffer.append(c)
            
            # Initial transition
            char_code = ord(c) if 0 <= ord(c) < 256 else 0
            next_state = self.YY_NXT[current_state][char_code]
            
            while next_state != -1:
                current_state = next_state
                
                if self.YY_ACCEPT[current_state] != -1:
                    last_accepting_state = current_state
                    last_accepting_idx = len(buffer)
                
                c = self.read_char()
                if c is None:
                    break
                
                if len(buffer) >= 4095:
                    sys.stderr.write("Buffer overflow\n")
                    sys.exit(1)
                
                buffer.append(c)
                char_code = ord(c) if 0 <= ord(c) < 256 else 0
                next_state = self.YY_NXT[current_state][char_code]
            
            # No more transitions
            if last_accepting_state != -1:
                # Match found! Backtrack.
                while len(buffer) > last_accepting_idx:
                    self.unread_char(buffer.pop())
                
                self.yytext = "".join(buffer)
                self.yyleng = len(buffer)
                
                match_id = self.YY_ACCEPT[last_accepting_state]
                
                # Create local aliases for user code access
                yytext = self.yytext
                yyleng = self.yyleng

__YYLEX_BODY_PLACEHOLDER__

            else:
                # No match. Echo first char and unput everything else.
                sys.stdout.write(buffer[0])
                sys.stdout.flush()
                while len(buffer) > 1:
                    self.unread_char(buffer.pop())

__USER_CODE_PLACEHOLDER__

if __name__ == '__main__':
    
    if len(sys.argv) > 1:
        try:
            with open(sys.argv[1], 'r') as f:
                lexer = Lexer(f)
                lexer.yylex()
        except FileNotFoundError:
            print(f"Error: Input file '{sys.argv[1]}' not found.", file=sys.stderr)
            sys.exit(1)
    else:
        # Read from standard input (stdin)
        lexer = Lexer(sys.stdin)
        lexer.yylex()