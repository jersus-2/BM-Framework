#ifndef OUT_AND_IN_HPP
#define OUT_AND_IN_HPP

namespace bm {
    struct uig_t {};
    #define uig uig_t

    struct _enl {};
    static _enl enl;

    // --- Morse Code Logic ---
    struct mc_map { const char* m; char c; };
    static const mc_map _mct[] = {
        {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'}, {".", 'E'},
        {"..-.", 'F'}, {"--.", 'G'}, {"....", 'H'}, {"..", 'I'}, {".---", 'J'},
        {"-.-", 'K'}, {".-..", 'L'}, {"--", 'M'}, {"-.", 'N'}, {"---", 'O'},
        {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'},
        {"..-", 'U'}, {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'}, {"-.--", 'Y'}, {"--..", 'Z'},
        {"-----", '0'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'}, {"....-", '4'},
        {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}
    };

    template<const char* M>
    static const char* mc() {
        static char result[256];
        int r_idx = 0, m_idx = 0;
        char buffer[10] = {0};
        int b_idx = 0;
        auto flush = [&]() {
            if (b_idx == 0) return;
            buffer[b_idx] = '\0';
            for (int i = 0; i < 36; i++) {
                bool match = true;
                for (int j = 0; buffer[j] || _mct[i].m[j]; j++) {
                    if (buffer[j] != _mct[i].m[j]) { match = false; break; }
                }
                if (match) { result[r_idx++] = _mct[i].c; break; }
            }
            b_idx = 0;
        };
        for (; M[m_idx]; m_idx++) {
            if (M[m_idx] == ' ') { flush(); }
            else if (M[m_idx] == '/') { flush(); result[r_idx++] = ' '; }
            else { buffer[b_idx++] = M[m_idx]; }
        }
        flush();
        result[r_idx] = '\0';
        return result;
    }

    namespace sys {
        static long call(long nr, long a0, long a1, long a2, long a3 = 0, long a4 = 0) {
            long ret;
            asm volatile ("mov x8, %1\n mov x0, %2\n mov x1, %3\n mov x2, %4\n mov x3, %5\n mov x4, %6\n svc #0\n mov %0, x0"
                : "=r"(ret) : "r"(nr), "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4) : "x0", "x1", "x2", "x3", "x4", "x8");
            return ret;
        }
    }

    namespace stf {
        class stg;
        template <typename T> T* nw(unsigned int c = 1) { return (T*)sys::call(222, 0, sizeof(T) * c, 3, 34); }
        template <typename T> void old(T*& p, unsigned int c = 1) { if (p) { sys::call(215, (long)p, sizeof(T) * c, 0); p = nullptr; } }

        class stg {
        public:
            char* data; unsigned int len;
            stg() : data(nullptr), len(0) {}
            stg(const char* s) {
                len = 0; if(!s) return;
                while(s[len]) len++;
                data = nw<char>(len + 1);
                for(unsigned int i=0; i<len; i++) data[i] = s[i];
                data[len] = '\0';
            }
            stg(char* b, unsigned int s) : data(b), len(s) {}
            ~stg() { if(data) old(data, len + 1); }
            stg& operator=(const stg& o) {
                if(data) old(data, len + 1);
                len = o.len; data = nw<char>(len + 1);
                for(unsigned int i=0; i<=len; i++) data[i] = o.data[i];
                return *this;
            }
        };

        struct _shee {
            const char* find(const char* n) { return n; }
            void write(const char* p, const char* t) { unsigned long l = 0; while(t[l]) l++;
                long fd = sys::call(56, -100, (long)p, 65, 0644); if(fd > 0) { sys::call(64, fd, (long)t, l); sys::call(57, fd, 0, 0); } }
            char* open(const char* p, unsigned int* sz = nullptr) { 
                long fd = sys::call(56, -100, (long)p, 0, 0); 
                if(fd < 0) return nullptr;
                char* b = nw<char>(4096); 
                long r = sys::call(63, fd, (long)b, 4096); 
                sys::call(57, fd, 0, 0);
                if (sz) *sz = (r > 0) ? (unsigned int)r : 0;
                if (r > 0 && r < 4096) b[r] = '\0';
                return b; 
            }
        };
        static _shee shee;
    }

    class _in {
    public:
        _in& operator>>(stf::stg& s) {
            char t[1024] = {0};
            long r = sys::call(63, 0, (long)t, 1024);
            if (r > 0) {
                for(int i=0; i<r; i++) { if(t[i] == '\n' || t[i] == '\r') { t[i] = '\0'; break; } }
            }
            s = stf::stg(t);
            return *this;
        }
    };
    static _in in;

    class _out {
    public:
        _out& operator<<(const char* s) { 
            unsigned long l = 0; if(s) while(s[l]) l++; 
            sys::call(64, 1, (long)s, l); return *this; 
        }
        _out& operator<<(const stf::stg& s) { 
            if(s.data) sys::call(64, 1, (long)s.data, s.len); return *this; 
        }
        _out& operator<<(_enl) { 
            sys::call(64, 1, (long)"\n", 1); return *this; 
        }
    };
    static _out out;
}

namespace pro {
    typedef bm::stf::stg stg;
    class _inout {
    public:
        _inout& operator<<(const char* s) { bm::out << s; return *this; }
        _inout& operator<<(const stg& s) { bm::out << s; return *this; }
        _inout& operator<<(bm::_enl e) { bm::out << e; return *this; }
        _inout& operator>>(stg& s) { 
            bm::in >> s;
            bm::out << "\033[A"; // Move Up
            for(unsigned int i=0; i < (s.len + 15); i++) bm::out << "\033[C"; // Move Right
            return *this; 
        }
    };
    static _inout inout;

    namespace stf {
        struct _shee_pro {
            const char* find(const char* n) { return bm::stf::shee.find(n); } // Fixed error
            stg open(const char* p) { 
                unsigned int s = 0; char* b = bm::stf::shee.open(p, &s); 
                return stg(b, s); 
            }
        };
        static _shee_pro shee;
    }
}
#endif
