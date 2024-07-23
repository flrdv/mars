//
// Created by pavlo on 12-Jun-24.
//


#ifndef MARS_HTTP_STATUSES_H
#define MARS_HTTP_STATUSES_H

#include <stdint.h>
#include <stddef.h>

#include "lib/slice.h"

typedef uint16_t http_status_t;

static const http_status_t HTTP_STATUS_CONTINUE            = 100;           // RFC 9110, 15.2.1
static const http_status_t HTTP_STATUS_SWITCHING_PROTOCOLS = 101; // RFC 9110, 15.2.2
static const http_status_t HTTP_STATUS_PROCESSING          = 102; // RFC 2518, 10.1
static const http_status_t HTTP_STATUS_EARLY_HINTS         = 103; // RFC 8297

static const http_status_t HTTP_STATUS_OK                     = 200; // RFC 9110, 15.3.1
static const http_status_t HTTP_STATUS_CREATED                = 201; // RFC 9110, 15.3.2
static const http_status_t HTTP_STATUS_ACCEPTED               = 202; // RFC 9110, 15.3.3
static const http_status_t HTTP_STATUS_NON_AUTHORITATIVE_INFO = 203; // RFC 9110, 15.3.4
static const http_status_t HTTP_STATUS_NO_CONTENT             = 204; // RFC 9110, 15.3.5
static const http_status_t HTTP_STATUS_RESET_CONTENT          = 205; // RFC 9110, 15.3.6
static const http_status_t HTTP_STATUS_PARTIAL_CONTENT        = 206; // RFC 9110, 15.3.7
static const http_status_t HTTP_STATUS_MULTI_STATUS           = 207; // RFC 4918, 11.1
static const http_status_t HTTP_STATUS_ALREADY_REPORTED       = 208; // RFC 5842, 7.1
static const http_status_t HTTP_STATUS_IM_USED                = 226; // RFC 3229, 10.4.1

static const http_status_t HTTP_STATUS_MULTIPLE_CHOICES   = 300; // RFC 9110, 15.4.1
static const http_status_t HTTP_STATUS_MOVED_PERMANENTLY  = 301; // RFC 9110, 15.4.2
static const http_status_t HTTP_STATUS_FOUND              = 302; // RFC 9110, 15.4.3
static const http_status_t HTTP_STATUS_SEE_OTHER          = 303; // RFC 9110, 15.4.4
static const http_status_t HTTP_STATUS_NOT_MODIFIED       = 304; // RFC 9110, 15.4.5
static const http_status_t HTTP_STATUS_USE_PROXY          = 305; // RFC 9110, 15.4.6
static const http_status_t HTTP_STATUS_TEMPORARY_REDIRECT = 307; // RFC 9110, 15.4.8
static const http_status_t HTTP_STATUS_PERMANENT_REDIRECT = 308; // RFC 9110, 15.4.9

static const http_status_t HTTP_STATUS_BAD_REQUEST                     = 400; // RFC 9110, 15.5.1
static const http_status_t HTTP_STATUS_UNAUTHORIZED                    = 401; // RFC 9110, 15.5.2
static const http_status_t HTTP_STATUS_PAYMENT_REQUIRED                = 402; // RFC 9110, 15.5.3
static const http_status_t HTTP_STATUS_FORBIDDEN                       = 403; // RFC 9110, 15.5.4
static const http_status_t HTTP_STATUS_NOT_FOUND                       = 404; // RFC 9110, 15.5.5
static const http_status_t HTTP_STATUS_METHOD_NOT_ALLOWED              = 405; // RFC 9110, 15.5.6
static const http_status_t HTTP_STATUS_NOT_ACCEPTABLE                  = 406; // RFC 9110, 15.5.7
static const http_status_t HTTP_STATUS_PROXY_AUTH_REQUIRED             = 407; // RFC 9110, 15.5.8
static const http_status_t HTTP_STATUS_REQUEST_TIMEOUT                 = 408; // RFC 9110, 15.5.9
static const http_status_t HTTP_STATUS_CONFLICT                        = 409; // RFC 9110, 15.5.10
static const http_status_t HTTP_STATUS_GONE                            = 410; // RFC 9110, 15.5.11
static const http_status_t HTTP_STATUS_LENGTH_REQUIRED                 = 411; // RFC 9110, 15.5.12
static const http_status_t HTTP_STATUS_PRECONDITION_FAILED             = 412; // RFC 9110, 15.5.13
static const http_status_t HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE        = 413; // RFC 9110, 15.5.14
static const http_status_t HTTP_STATUS_REQUEST_URI_TOO_LONG            = 414; // RFC 9110, 15.5.15
static const http_status_t HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE          = 415; // RFC 9110, 15.5.16
static const http_status_t HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416; // RFC 9110, 15.5.17
static const http_status_t HTTP_STATUS_EXPECTATION_FAILED              = 417; // RFC 9110, 15.5.18
static const http_status_t HTTP_STATUS_TEAPOT                          = 418; // RFC 9110, 15.5.19 (Unused)
static const http_status_t HTTP_STATUS_MISDIRECTED_REQUEST             = 421; // RFC 9110, 15.5.20
static const http_status_t HTTP_STATUS_UNPROCESSABLE_ENTITY            = 422; // RFC 9110, 15.5.21
static const http_status_t HTTP_STATUS_LOCKED                          = 423; // RFC 4918, 11.3
static const http_status_t HTTP_STATUS_FAILED_DEPENDENCY               = 424; // RFC 4918, 11.4
static const http_status_t HTTP_STATUS_TOO_EARLY                       = 425; // RFC 8470, 5.2.
static const http_status_t HTTP_STATUS_UPGRADE_REQUIRED                = 426; // RFC 9110, 15.5.22
static const http_status_t HTTP_STATUS_PRECONDITION_REQUIRED           = 428; // RFC 6585, 3
static const http_status_t HTTP_STATUS_TOO_MANY_REQUESTS               = 429; // RFC 6585, 4
static const http_status_t HTTP_STATUS_HEADER_FIELDS_TOO_LARGE         = 431; // RFC 6585, 5
static const http_status_t HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS   = 451; // RFC 7725, 3
static const http_status_t HTTP_STATUS_BROKEN_PIPE                     = 499; // used internally

static const http_status_t HTTP_STATUS_INTERNAL_SERVER_ERROR           = 500; // RFC 9110, 15.6.1
static const http_status_t HTTP_STATUS_NOT_IMPLEMENTED                 = 501; // RFC 9110, 15.6.2
static const http_status_t HTTP_STATUS_BAD_GATEWAY                     = 502; // RFC 9110, 15.6.3
static const http_status_t HTTP_STATUS_SERVICE_UNAVAILABLE             = 503; // RFC 9110, 15.6.4
static const http_status_t HTTP_STATUS_GATEWAY_TIMEOUT                 = 504; // RFC 9110, 15.6.5
static const http_status_t HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED      = 505; // RFC 9110, 15.6.6
static const http_status_t HTTP_STATUS_VARIANT_ALSO_NEGOTIATES         = 506; // RFC 2295, 8.1
static const http_status_t HTTP_STATUS_INSUFFICIENT_STORAGE            = 507; // RFC 4918, 11.5
static const http_status_t HTTP_STATUS_LOOP_DETECTED                   = 508; // RFC 5842, 7.2
static const http_status_t HTTP_STATUS_NOT_EXTENDED                    = 510; // RFC 2774, 7
static const http_status_t HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511; // RFC 6585, 6

#define S(str, l) (slice_t) { .ptr = (byte_t*)str, .len = l }

static const slice_t HTTP_STATUS_TOSTR[] = {
    [HTTP_STATUS_CONTINUE]                        = S("100 Continue\r\n",                        14),
    [HTTP_STATUS_SWITCHING_PROTOCOLS]             = S("101 Switching Protocols\r\n",             25),
    [HTTP_STATUS_PROCESSING]                      = S("102 Processing\r\n",                      16),
    [HTTP_STATUS_EARLY_HINTS]                     = S("103 Early Hints\r\n",                     17),
    [HTTP_STATUS_OK]                              = S("200 OK\r\n",                              8),
    [HTTP_STATUS_CREATED]                         = S("201 Created\r\n",                         13),
    [HTTP_STATUS_ACCEPTED]                        = S("202 Accepted\r\n",                        14),
    [HTTP_STATUS_NON_AUTHORITATIVE_INFO]          = S("203 Non-Authoritative Info\r\n",          28),
    [HTTP_STATUS_NO_CONTENT]                      = S("204 No Content\r\n",                      16),
    [HTTP_STATUS_RESET_CONTENT]                   = S("205 Reset Content\r\n",                   19),
    [HTTP_STATUS_PARTIAL_CONTENT]                 = S("206 Partial Content\r\n",                 21),
    [HTTP_STATUS_MULTI_STATUS]                    = S("207 Multi-Status\r\n",                    18),
    [HTTP_STATUS_ALREADY_REPORTED]                = S("208 Already Reported\r\n",                22),
    [HTTP_STATUS_IM_USED]                         = S("226 I'm Used\r\n",                        14),
    [HTTP_STATUS_MULTIPLE_CHOICES]                = S("300 Multiple Choices\r\n",                22),
    [HTTP_STATUS_MOVED_PERMANENTLY]               = S("301 Moved Permanently\r\n",               23),
    [HTTP_STATUS_FOUND]                           = S("302 Found\r\n",                           11),
    [HTTP_STATUS_SEE_OTHER]                       = S("303 See Other\r\n",                       15),
    [HTTP_STATUS_NOT_MODIFIED]                    = S("304 Not Modified\r\n",                    18),
    [HTTP_STATUS_USE_PROXY]                       = S("305 Use Proxy\r\n",                       15),
    [HTTP_STATUS_TEMPORARY_REDIRECT]              = S("307 Temporary Redirect\r\n",              24),
    [HTTP_STATUS_PERMANENT_REDIRECT]              = S("308 Permanent Redirect\r\n",              24),
    [HTTP_STATUS_BAD_REQUEST]                     = S("400 Bad Request\r\n",                     17),
    [HTTP_STATUS_UNAUTHORIZED]                    = S("401 Unauthorized\r\n",                    18),
    [HTTP_STATUS_PAYMENT_REQUIRED]                = S("402 Payment Required\r\n",                22),
    [HTTP_STATUS_FORBIDDEN]                       = S("403 Forbidden\r\n",                       15),
    [HTTP_STATUS_NOT_FOUND]                       = S("404 Not Found\r\n",                       15),
    [HTTP_STATUS_METHOD_NOT_ALLOWED]              = S("405 Method Not Allowed\r\n",              24),
    [HTTP_STATUS_NOT_ACCEPTABLE]                  = S("406 Not Acceptable\r\n",                  20),
    [HTTP_STATUS_PROXY_AUTH_REQUIRED]             = S("407 Proxy Auth Required\r\n",             25),
    [HTTP_STATUS_REQUEST_TIMEOUT]                 = S("408 Request Timeout\r\n",                 21),
    [HTTP_STATUS_CONFLICT]                        = S("409 Conflict\r\n",                        14),
    [HTTP_STATUS_GONE]                            = S("410 Gone\r\n",                            10),
    [HTTP_STATUS_LENGTH_REQUIRED]                 = S("411 Length Required\r\n",                 21),
    [HTTP_STATUS_PRECONDITION_FAILED]             = S("412 Precondition Failed\r\n",             25),
    [HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE]        = S("413 Request Entity Too Large\r\n",        30),
    [HTTP_STATUS_REQUEST_URI_TOO_LONG]            = S("414 Request URI Too Long\r\n",            26),
    [HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE]          = S("415 Unsupported Media Type\r\n",          28),
    [HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE] = S("416 Requested Range Not Satisfiable\r\n", 37),
    [HTTP_STATUS_EXPECTATION_FAILED]              = S("417 Expectation Failed\r\n",              24),
    [HTTP_STATUS_TEAPOT]                          = S("418 Teapot\r\n",                          12),
    [HTTP_STATUS_MISDIRECTED_REQUEST]             = S("421 Misdirected Request\r\n",             25),
    [HTTP_STATUS_UNPROCESSABLE_ENTITY]            = S("422 Unprocessable Entity\r\n",            26),
    [HTTP_STATUS_LOCKED]                          = S("423 Locked\r\n",                          12),
    [HTTP_STATUS_FAILED_DEPENDENCY]               = S("424 Failed Dependency\r\n",               23),
    [HTTP_STATUS_TOO_EARLY]                       = S("425 Too Early\r\n",                       15),
    [HTTP_STATUS_UPGRADE_REQUIRED]                = S("426 Upgrade Required\r\n",                22),
    [HTTP_STATUS_PRECONDITION_REQUIRED]           = S("428 Precondition Required\r\n",           27),
    [HTTP_STATUS_TOO_MANY_REQUESTS]               = S("429 Too Many Requests\r\n",               23),
    [HTTP_STATUS_HEADER_FIELDS_TOO_LARGE]         = S("431 Header Fields Too Large\r\n",         29),
    [HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS]   = S("451 Unavailable For Legal Reasons\r\n",   35),
    [HTTP_STATUS_BROKEN_PIPE]                     = S("499 Broken Pipe\r\n",                     17),
    [HTTP_STATUS_INTERNAL_SERVER_ERROR]           = S("500 Internal Server Error\r\n",           27),
    [HTTP_STATUS_NOT_IMPLEMENTED]                 = S("501 Not Implemented\r\n",                 21),
    [HTTP_STATUS_BAD_GATEWAY]                     = S("502 Bad Gateway\r\n",                     17),
    [HTTP_STATUS_SERVICE_UNAVAILABLE]             = S("503 Service Unavailable\r\n",             25),
    [HTTP_STATUS_GATEWAY_TIMEOUT]                 = S("504 Gateway Timeout\r\n",                 21),
    [HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED]      = S("505 HTTP Version Not Supported\r\n",      32),
    [HTTP_STATUS_VARIANT_ALSO_NEGOTIATES]         = S("506 Variant Also Negotiates\r\n",         29),
    [HTTP_STATUS_INSUFFICIENT_STORAGE]            = S("507 Insufficient Storage\r\n",            26),
    [HTTP_STATUS_LOOP_DETECTED]                   = S("508 Loop Detected\r\n",                   19),
    [HTTP_STATUS_NOT_EXTENDED]                    = S("510 Not Extended\r\n",                    18),
    [HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED] = S("511 Network Authentication Required\r\n", 37),
};

static const size_t HTTP_STATUS_TOSTR_LEN = sizeof(HTTP_STATUS_TOSTR)/sizeof(HTTP_STATUS_TOSTR[0]);

#endif // MARS_HTTP_STATUSES_H
