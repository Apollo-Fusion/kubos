/*
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cmocka.h>
#include <tinycbor/cbor.h>


static void test_encode_csp_packet(void ** arg)
{
    char buffer[100];
    CborEncoder encoder, container;

}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_encode_csp_packet)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}