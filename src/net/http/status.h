//
// Created by floordiv on 12-Jun-24.
//

#include <stdint.h>

#ifndef MARS_HTTP_STATUSES_H
#define MARS_HTTP_STATUSES_H

typedef uint16_t http_status_t;

const http_status_t HTTP_STATUS_CONTINUE = 100;; // RFC 9110, 15.2.1
const http_status_t HTTP_STATUS_SWITCHING_PROTOCOLS  = 101; // RFC 9110, 15.2.2
const http_status_t HTTP_STATUS_PROCESSING          = 102; // RFC 2518, 10.1
const http_status_t HTTP_STATUS_EARLY_HINTS          = 103; // RFC 8297

const http_status_t HTTP_STATUS_OK                    = 200; // RFC 9110, 15.3.1
const http_status_t HTTP_STATUS_CREATED               = 201; // RFC 9110, 15.3.2
const http_status_t HTTP_STATUS_ACCEPTED              = 202; // RFC 9110, 15.3.3
const http_status_t HTTP_STATUS_NON_AUTHORITATIVE_INFO  = 203; // RFC 9110, 15.3.4
const http_status_t HTTP_STATUS_NO_CONTENT             = 204; // RFC 9110, 15.3.5
const http_status_t HTTP_STATUS_RESET_CONTENT          = 205; // RFC 9110, 15.3.6
const http_status_t HTTP_STATUS_PARTIAL_CONTENT        = 206; // RFC 9110, 15.3.7
const http_status_t HTTP_STATUS_MULTI_STATUS           = 207; // RFC 4918, 11.1
const http_status_t HTTP_STATUS_ALREADY_REPORTED       = 208; // RFC 5842, 7.1
const http_status_t HTTP_STATUS_IM_USED                = 226; // RFC 3229, 10.4.1

const http_status_t HTTP_STATUS_MULTIPLE_CHOICES    = 300; // RFC 9110, 15.4.1
const http_status_t HTTP_STATUS_MOVED_PERMANENTLY   = 301; // RFC 9110, 15.4.2
const http_status_t HTTP_STATUS_FOUND              = 302; // RFC 9110, 15.4.3
const http_status_t HTTP_STATUS_SEE_OTHER           = 303; // RFC 9110, 15.4.4
const http_status_t HTTP_STATUS_NOT_MODIFIED        = 304; // RFC 9110, 15.4.5
const http_status_t HTTP_STATUS_USE_PROXY           = 305; // RFC 9110, 15.4.6
const http_status_t HTTP_STATUS_TEMPORARY_REDIRECT  = 307; // RFC 9110, 15.4.8
const http_status_t HTTP_STATUS_PERMANENT_REDIRECT  = 308; // RFC 9110, 15.4.9

const http_status_t HTTP_STATUS_BAD_REQUEST                    = 400; // RFC 9110, 15.5.1
const http_status_t HTTP_STATUS_UNAUTHORIZED                  = 401; // RFC 9110, 15.5.2
const http_status_t HTTP_STATUS_PAYMENT_REQUIRED               = 402; // RFC 9110, 15.5.3
const http_status_t HTTP_STATUS_FORBIDDEN                     = 403; // RFC 9110, 15.5.4
const http_status_t HTTP_STATUS_NOT_FOUND                      = 404; // RFC 9110, 15.5.5
const http_status_t HTTP_STATUS_METHOD_NOT_ALLOWED              = 405; // RFC 9110, 15.5.6
const http_status_t HTTP_STATUS_NOT_ACCEPTABLE                 = 406; // RFC 9110, 15.5.7
const http_status_t HTTP_STATUS_PROXY_AUTH_REQUIRED             = 407; // RFC 9110, 15.5.8
const http_status_t HTTP_STATUS_REQUEST_TIMEOUT                = 408; // RFC 9110, 15.5.9
const http_status_t HTTP_STATUS_CONFLICT                      = 409; // RFC 9110, 15.5.10
const http_status_t HTTP_STATUS_GONE                          = 410; // RFC 9110, 15.5.11
const http_status_t HTTP_STATUS_LENGTH_REQUIRED                = 411; // RFC 9110, 15.5.12
const http_status_t HTTP_STATUS_PRECONDITION_FAILED            = 412; // RFC 9110, 15.5.13
const http_status_t HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE         = 413; // RFC 9110, 15.5.14
const http_status_t HTTP_STATUS_REQUEST_URI_TOO_LONG             = 414; // RFC 9110, 15.5.15
const http_status_t HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE          = 415; // RFC 9110, 15.5.16
const http_status_t HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE  = 416; // RFC 9110, 15.5.17
const http_status_t HTTP_STATUS_EXPECTATION_FAILED             = 417; // RFC 9110, 15.5.18
const http_status_t HTTP_STATUS_TEAPOT                        = 418; // RFC 9110, 15.5.19 (Unused)
const http_status_t HTTP_STATUS_MISDIRECTED_REQUEST            = 421; // RFC 9110, 15.5.20
const http_status_t HTTP_STATUS_UNPROCESSABLE_ENTITY           = 422; // RFC 9110, 15.5.21
const http_status_t HTTP_STATUS_LOCKED                        = 423; // RFC 4918, 11.3
const http_status_t HTTP_STATUS_FAILED_DEPENDENCY              = 424; // RFC 4918, 11.4
const http_status_t HTTP_STATUS_TOO_EARLY                      = 425; // RFC 8470, 5.2.
const http_status_t HTTP_STATUS_UPGRADE_REQUIRED               = 426; // RFC 9110, 15.5.22
const http_status_t HTTP_STATUS_PRECONDITION_REQUIRED          = 428; // RFC 6585, 3
const http_status_t HTTP_STATUS_TOO_MANY_REQUESTS               = 429; // RFC 6585, 4
const http_status_t HTTP_STATUS_HEADER_FIELDS_TOO_LARGE          = 431; // RFC 6585, 5
const http_status_t HTTP_STATUS_CLOSE_CONNECTION               = 439; // internally used code
const http_status_t HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS    = 451; // RFC 7725, 3

const http_status_t HTTP_STATUS_INTERNAL_SERVER_ERROR            = 500; // RFC 9110, 15.6.1
const http_status_t HTTP_STATUS_NOT_IMPLEMENTED                 = 501; // RFC 9110, 15.6.2
const http_status_t HTTP_STATUS_BAD_GATEWAY                     = 502; // RFC 9110, 15.6.3
const http_status_t HTTP_STATUS_SERVICE_UNAVAILABLE             = 503; // RFC 9110, 15.6.4
const http_status_t HTTP_STATUS_GATEWAY_TIMEOUT                 = 504; // RFC 9110, 15.6.5
const http_status_t HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED      = 505; // RFC 9110, 15.6.6
const http_status_t HTTP_STATUS_VARIANT_ALSO_NEGOTIATES         = 506; // RFC 2295, 8.1
const http_status_t HTTP_STATUS_INSUFFICIENT_STORAGE            = 507; // RFC 4918, 11.5
const http_status_t HTTP_STATUS_LOOP_DETECTED                   = 508; // RFC 5842, 7.2
const http_status_t HTTP_STATUS_NOT_EXTENDED                    = 510; // RFC 2774, 7
const http_status_t HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511; // RFC 6585, 6

#endif // MARS_HTTP_STATUSES_H
