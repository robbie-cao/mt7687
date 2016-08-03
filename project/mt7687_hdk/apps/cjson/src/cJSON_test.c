/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

/**
  * @brief     Parse text to JSON, then render back in text, and print
  * @param[in] char *text:input string
  * @return    None
  */
void doit(char *text)
{
    char *out;
    cJSON *json;

    json = cJSON_Parse(text);
    if (!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    } else {
        out = cJSON_Print(json);
        cJSON_Delete(json);
        printf("%s\n", out);
        cJSON_free(out);
    }
}


/* Example data type used in this example project. */
struct record {
    const char *precision;
    double lat, lon;
    const char *address, *city, *state, *zip, *country;
};

/**
  * @brief     Create a number of JSON objects for demonstration purpose
  * @param     None
  * @return    None
  */
void create_objects()
{
    cJSON *root, *fmt, *img, *thm, *fld;
    char *out;
    int i;	/* The index number. */

    /* Here we construct several JSON objects. */

    /* The "Video" data type: */
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
    cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());
    cJSON_AddStringToObject(fmt, "type",		"rect");
    cJSON_AddNumberToObject(fmt, "width",		1920);
    cJSON_AddNumberToObject(fmt, "height",		1080);
    cJSON_AddFalseToObject (fmt, "interlace");
    cJSON_AddNumberToObject(fmt, "frame rate",	24);

    out = cJSON_Print(root); /* Print to text */
    cJSON_Delete(root);      /* Delete the cJSON object */
    printf("%s\n", out);     /* Print out the text */
    cJSON_free(out);	     /* Release the string. */

    /* The "days of the week" array: */
    const char *strings[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    root = cJSON_CreateStringArray(strings, 7);

    out = cJSON_Print(root);
    cJSON_Delete(root);
    printf("%s\n", out);
    cJSON_free(out);

    /* The matrix: */
    int numbers[3][3] = {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}};
    root = cJSON_CreateArray();
    for (i = 0; i < 3; i++) {
        cJSON_AddItemToArray(root, cJSON_CreateIntArray(numbers[i], 3));
    }

    /*	cJSON_ReplaceItemInArray(root,1,cJSON_CreateString("Replacement")); */
    out = cJSON_Print(root);
    cJSON_Delete(root);
    printf("%s\n", out);
    cJSON_free(out);

    /* The "gallery" item: */
    int ids[4] = {116, 943, 234, 38793};
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "Image", img = cJSON_CreateObject());
    cJSON_AddNumberToObject(img, "Width", 800);
    cJSON_AddNumberToObject(img, "Height", 600);
    cJSON_AddStringToObject(img, "Title", "View from 15th Floor");
    cJSON_AddItemToObject(img, "Thumbnail", thm = cJSON_CreateObject());
    cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
    cJSON_AddNumberToObject(thm, "Height", 125);
    cJSON_AddStringToObject(thm, "Width", "100");
    cJSON_AddItemToObject(img, "IDs", cJSON_CreateIntArray(ids, 4));

    out = cJSON_Print(root);
    cJSON_Delete(root);
    printf("%s\n", out);
    cJSON_free(out);

    /* The array of "records": */
    struct record fields[2] = {
        {"zip", 37.7668, -1.223959e+2, "", "SAN FRANCISCO", "CA", "94107", "US"},
        {"zip", 37.371991, -1.22026e+2, "", "SUNNYVALE", "CA", "94085", "US"}
    };

    root = cJSON_CreateArray();
    for (i = 0; i < 2; i++) {
        cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
        cJSON_AddStringToObject(fld, "precision", fields[i].precision);
        cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
        cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
        cJSON_AddStringToObject(fld, "Address", fields[i].address);
        cJSON_AddStringToObject(fld, "City", fields[i].city);
        cJSON_AddStringToObject(fld, "State", fields[i].state);
        cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
        cJSON_AddStringToObject(fld, "Country", fields[i].country);
    }

    /*	cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root,1),"City",cJSON_CreateIntArray(ids,4)); */
    out = cJSON_Print(root);
    cJSON_Delete(root);
    printf("%s\n", out);
    cJSON_free(out);

}

/* a bunch of json: */
char text1[] = "{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}";
char text2[] = "[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]";
char text3[] = "[\n    [0, -1, 0],\n    [1, 0, 0],\n    [0, 0, 1]\n   ]\n";
char text4[] = "{\n     \"Image\": {\n          \"Width\":  800,\n          \"Height\": 600,\n          \"Title\":  \"View from 15th Floor\",\n         \"Thumbnail\": {\n              \"Url\":    \"http:/*www.example.com/image/481989943\",\n               \"Height\": 125,\n              \"Width\":  \"100\"\n           },\n            \"IDs\": [116, 943, 234, 38793]\n       }\n }";
char text5[] = "[\n  {\n     \"precision\": \"zip\",\n   \"Latitude\":  37.7668,\n   \"Longitude\": -122.3959,\n     \"Address\":   \"\",\n  \"City\":      \"SAN FRANCISCO\",\n     \"State\":     \"CA\",\n    \"Zip\":       \"94107\",\n     \"Country\":   \"US\"\n     },\n    {\n     \"precision\": \"zip\",\n   \"Latitude\":  37.371991,\n     \"Longitude\": -122.026020,\n   \"Address\":   \"\",\n  \"City\":      \"SUNNYVALE\",\n     \"State\":     \"CA\",\n    \"Zip\":       \"94085\",\n     \"Country\":   \"US\"\n     }\n     ]";

/**
  * @brief     Cjson test entry
  * @param     None
  * @return    0 -> Success
  */
int cjson_test(void)
{

    /* Parse each JSON text block, then rebuild them back to JSON object and print out. */
    doit(text1);
    doit(text2);
    doit(text3);
    doit(text4);
    doit(text5);

    /* Execute the example code to build a number of JSON objects. */
    create_objects();
    return 0;
}
