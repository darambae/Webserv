
enum HttpStatus {
    // Informational responses (100–199)
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    PROCESSING = 102,                // WebDAV
    EARLY_HINTS = 103,

    // Successful responses (200–299)
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NON_AUTHORITATIVE_INFORMATION = 203,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTI_STATUS = 207,              // WebDAV
    ALREADY_REPORTED = 208,          // WebDAV
    IM_USED = 226,                   // HTTP Delta Encoding

    // Redirection messages (300–399)
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,                     // Previously "Moved Temporarily"
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    USE_PROXY = 305,                 // Deprecated
    TEMPORARY_REDIRECT = 307,
    PERMANENT_REDIRECT = 308,

    // Client error responses (400–499)
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    PAYMENT_REQUIRED = 402,          // Reserved for future use
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    IM_A_TEAPOT = 418,               // Easter Egg (RFC 2324)
    MISDIRECTED_REQUEST = 421,       // HTTP/2
    UNPROCESSABLE_ENTITY = 422,      // WebDAV
    LOCKED = 423,                    // WebDAV
    FAILED_DEPENDENCY = 424,         // WebDAV
    TOO_EARLY = 425,                 // Experimental
    UPGRADE_REQUIRED = 426,
    PRECONDITION_REQUIRED = 428,     // RFC 6585
    TOO_MANY_REQUESTS = 429,         // RFC 6585
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431, // RFC 6585
    UNAVAILABLE_FOR_LEGAL_REASONS = 451,   // RFC 7725

    // Server error responses (500–599)
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505,
    VARIANT_ALSO_NEGOTIATES = 506,   // RFC 2295
    INSUFFICIENT_STORAGE = 507,      // WebDAV
    LOOP_DETECTED = 508,             // WebDAV
    NOT_EXTENDED = 510,              // RFC 2774
    NETWORK_AUTHENTICATION_REQUIRED = 511 // RFC 6585
};
