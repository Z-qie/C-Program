/************************************ Please note this: *********************************************
 * For readability of 2D-map, please bother to kindly adjust terminal preference:                   *
 * 1. built-in scheme as "white on black";                                                          *
 * 2. ambiguous-width characters as "narrow" ;                                                      *
 * 3. encoding as "Unicode - UTF - 8".                                                              *
 * Thanks!                                                                                          *
 ****************************************************************************************************/
/****************************************************************************************************
 FYR

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <wchar.h>
#include <locale.h>

#define LENGTH 15                                               // the max length for target info.                               
#define EPS 0.00000000000001                                    // epsilon 

typedef struct target{                                          // linked list data structure declaration.
    char   name[ LENGTH + 1 ];
    double latitude;
    double longtitude;
    struct target *next;
} Target;

*****************************************************************************************************/


void printMap( Target *head )  
{          
// in case the terminal doesn't use scheme as "white on black", already set the background as black.
// customized char font in map.
    #define NORMAL  "\033[0m"                                   // normal type to resit.                                              
    #define GREY    "\033[3;38;5;241;48;5;16m"                  // italic grey char type.
    #define COLOR   "\033[48;5;16;38;5;"                        // bright colored char type (to be shaded later).
    #define LABEL   "\033[1;2;48;5;236m"                        // special label font.
    #define TAGBAR  "\033[1;4;38;5;239;48;5;16m"                // bold font for tagbar.
    #define BLINK   "\033[3;5;38;5;241;48;5;16m"                // blink font for limit prompt.
    #define ANTIWAR "\033[1;5;38;5;1;48;5;16m"                  // blink font for ANTIWAR prompt.

    setlocale( LC_ALL, "en_US.UTF-8" );                         // set locale to UTF-8 in order to use unicode char.

    struct Pixel                                                // treat the whole map as a 40*80 area pixel map.  
    {                                                           // set a structure to hold each pixel:
       wchar_t pixel;                                           // 1. a char as a pixel entity.
       char font[ 30 ];                                         // 2. a string to denote output form of each pixel.
    }map[ 40 ][ 80 ];  

//========================================= initialize entire frame ===============================================================
    for( int row = 0; row < 40; ++row )                         // initialization of entire map box.
    {                                                           
        for( int col = 0; col < 80; ++col )
        {
            map[ row ][ col ].pixel = L' ';                     // initialize all pixel as a space with a greyish font.
            strcpy( map[ row ][ col ].font, GREY);
                                                                // draw horizontal double-lines of the box frame.
            map[ 0 ][ col ].pixel = map[ 39 ][ col ].pixel =  L'\u2550'; 
        }
                                                                // draw vertical double-lines of the box frame.   
        map[ row ][ 0 ].pixel = map[ row ][ 79 ].pixel = L'\u2551';              
    }
    
    for( int col = 1; col < 79; ++col )                         // draw mid dividing line.
        map[ 28 ][ col ].pixel = L'\u2500';
    
    map[ 0 ][ 0 ].pixel   = L'\u2554';                          // set 4 angles of box frame.
    map[ 0 ][ 79 ].pixel  = L'\u2557';                              
    map[ 39 ][ 0 ].pixel  = L'\u255A';
    map[ 39 ][ 79 ].pixel = L'\u255D';
    map[ 28 ][ 0 ].pixel  = L'\u255F';                          // set 2 joints to connecting dividing line of the frame.
    map[ 28 ][ 79 ].pixel = L'\u2562';  

//========================================= initialize 2D-map =====================================================================

    const wchar_t title[] = L"\u25B6 2D MAP - PLANET E \u25C0"; // initialize map title as string.
    for( int col = 19; col < 19 + wcslen( title ); ++col )
    {
        map[ 1 ][ col ].pixel = title[ col - 19 ];              // convert map title into pixel map with label font.
        strcpy( map[ 1 ][ col ].font, LABEL );
    }

    const wchar_t antiwar[] = L"\u262E ANTI WAR \u262E";           // initialize antiwar as string.
    for( int col = 60; col < 60 + wcslen( antiwar ); ++col )
    {
        map[ 1 ][ col ].pixel = antiwar[ col - 60 ];             // convert label into pixel map in label font.       
        strcpy( map[ 1 ][ col ].font, ANTIWAR );
    }
                                                                // initializa map frame:
    for( int row = 2; row < 27; row++ )                         // 1. draw horizontal double-lines of the map frame.
        map[ row ][ 4 ].pixel = map[ row ][ 53 ].pixel = L'\u2551';  
                                                                
    for( int row = 6; row < 27; row += 5)                       // 2. draw all horizontal dash-lines of the map grid.
    {
        map[ row ][ 4 ].pixel = L'\u255F';                      //    left-hand side joints with frame. 
        map[ row ][ 53 ].pixel = L'\u2562';                     //    right-hand side joints with frame. 
        
        for( int col = 5; col < 53; ++col)                      //    dash-lines inside the frame.
            map[ row ][ col ].pixel = L'\u2508';                
    }
    
    for( int col = 5; col < 54; col++ )                         // 3. draw all vertical double-lines of the map grid.
        map[ 2 ][ col ].pixel = map[ 26 ][ col ].pixel = L'\u2550'; 

    for( int col = 14; col < 54; col += 10)                     // 4. draw all vertical dash-lines of the map grid.
    {
        map[ 2 ][ col ].pixel = L'\u2564';                      //    joints with frame at the top line. 
        map[ 26 ][ col ].pixel = L'\u2567';                     //    joints with frame at the bottom line. 

        for( int row = 3; row < 26; ++row)                      //    dash-lines inside the frame. 
            map[ row ][ col ].pixel = ( map[ row ][ col ].pixel == L'\u2508' ) ?  L'\u253C' : L'\u250A';
    }                                                           //    if dash-lines exist, change them as joints. 
      
    map[ 2 ][ 4 ].pixel   = L'\u2554';                          // 5. set 4 angles of map frame.
    map[ 2 ][ 53 ].pixel  = L'\u2557';                              
    map[ 26 ][ 4 ].pixel  = L'\u255A';
    map[ 26 ][ 53 ].pixel = L'\u255D'; 


    map[ 1 ][ 2 ].pixel   = L'\u1D38';                          // 6.set coordinate references with small-size fonts.
    map[ 1 ][ 3 ].pixel   = L'\u1D52';                          // Longtitude label as Lo.
    map[ 27 ][ 56 ].pixel = L'\u1D38';                          // Latitude label as La.
    map[ 27 ][ 57 ].pixel = L'\u1D45';

    map[ 2 ][ 1 ].pixel  = L'\u2081';                           // 7.set values of longtitude with small-size fonts.
    map[ 2 ][ 2 ].pixel  = map[ 2 ][ 3 ].pixel = L'\u2080';     // 100.
    map[ 6 ][ 2 ].pixel  = L'\u2088';                           // 80.
    map[ 6 ][ 3 ].pixel  = L'\u2080';
    map[ 11 ][ 2 ].pixel = L'\u2086';                           // 60.
    map[ 11 ][ 3 ].pixel = L'\u2080';    
    map[ 16 ][ 2 ].pixel = L'\u2084';                           // 40.
    map[ 16 ][ 3 ].pixel = L'\u2080';  
    map[ 21 ][ 2 ].pixel = L'\u2082';                           // 20.
    map[ 21 ][ 3 ].pixel = L'\u2080';                           
    map[ 26 ][ 2 ].pixel = L'\u2080';                           // 0.

    map[ 27 ][ 52 ].pixel = L'\u2081';                          // 8.set values of latitude with small-size fonts.
    map[ 27 ][ 53 ].pixel = map[ 27 ][ 54 ].pixel = L'\u2080';  // 100.
    map[ 27 ][ 44 ].pixel = L'\u2088';                          // 80.
    map[ 27 ][ 45 ].pixel = L'\u2080';
    map[ 27 ][ 34 ].pixel = L'\u2086';                          // 60.
    map[ 27 ][ 35 ].pixel = L'\u2080';
    map[ 27 ][ 24 ].pixel = L'\u2084';                          // 40.
    map[ 27 ][ 25 ].pixel = L'\u2080';
    map[ 27 ][ 14 ].pixel = L'\u2082';                          // 20.
    map[ 27 ][ 15 ].pixel = L'\u2080';
    map[ 27 ][ 4 ].pixel  = L'\u2080';                          // 0.

//========================================= initialize legend card ================================================================

    const wchar_t legend[] = L"\u25B6 LEGEND \u25C0";           // initialize legend as string.
    for( int col = 61; col < 61 + wcslen( legend ); ++col )
    {
        map[ 3 ][ col ].pixel = legend[ col - 61 ];             // convert label into pixel map in label font.       
        strcpy( map[ 3 ][ col ].font, LABEL );
    }

    const wchar_t legend_tag[] = L" Tag La-Range Lo-Range ";    // set tag bar as string.
    for( int col = 56; col < 54 + wcslen( legend_tag ); ++col )
    {
        map[ 5 ][ col ].pixel = legend_tag[ col - 55 ];         // convert tag bar string into pixel map in tagbar font.             
        strcpy( map[ 5 ][ col ].font, TAGBAR );
    }                                    
                                                                // initializa legend frame:   
    for( int col = 56; col < 77; ++col )                        // 1. draw horizontal dash-lines of the legend frame.
        map[ 2 ][ col ].pixel = map[ 26 ][ col ].pixel = L'\u2504';                  
                
    for( int row = 3; row < 26; ++row )                         // 2. draw vertical dash-lines of the legend grid.
        map[ row ][ 55 ].pixel = map[ row ][ 77 ].pixel = L'\u2506';   

    map[ 2 ][ 55 ].pixel  = L'\u250C';                          // 3. set 4 angles of map frame.
    map[ 2 ][ 77 ].pixel  = L'\u2510';  
    map[ 26 ][ 55 ].pixel = L'\u2514';  
    map[ 26 ][ 77 ].pixel = L'\u2518';  

//========================================= initialize target-info card ===========================================================
    
    const wchar_t info_title[] = L" TARGET INFO ";              // initialize label as string.
    for( int col = 4; col < 17; ++col )                         // convert label into pixel map in label font.       
    {
            map[ 28 ][ col ].pixel = info_title[ col - 4 ]; 
            strcpy( map[ 28 ][ col ].font, LABEL );                    
    }
                                                                // set tag bar as string.
    const wchar_t info_tag[] = L"Tag Name       Latitude   Longtitude   Tag Name       Latitude   Longtitude";  
    for( int col = 2; col < 2 + wcslen( info_tag ); ++col )     // convert tag bar string into pixel map in tagbar font.      
    {
            map[ 29 ][ col ].pixel = info_tag[ col - 2 ];
            strcpy( map[ 29 ][ col ].font, TAGBAR );                    
    }

    for( int row = 29; row < 39; ++row )                        // draw mid dividing line of the target info card.
    {
        map[ row ][ 39 ].pixel = L'\u2502';    
        strcpy( map[ row ][ 39 ].font, GREY );               
    }

//========================================= start scanning and targeting ==========================================================
    
    Target *cur = head;                                         // set pointer of target.
    
    int target_count = 0;                                       // initialize a counter of targets.
    int tag_row = 6;                                            // initialize a counter of capacity of our legend card.

    while( cur != NULL )                                        // scanning..
    {
        target_count++;
        int x = ( int )cur->latitude / 2 + 4;                   // calculate coordinate corresponding to the pixel map.
        int y = 26 - ( int )cur->longtitude / 4 ;
        y = ( y == 1 ) ? 2 : y;                                 
        x = ( x == 54 ) ? 53 : x;  
                                                                // 1.Map: set corresponding pixels as new targets with new tags if haven't been defined
        if( !strcmp( map[ y ][ x ].font, GREY ) )
        {                                                       // initialize a counter of targets.
            map[ y ][ x ].pixel = ( L'\u24B5' + target_count ); // set a new tag to corresponding pixel with colored font.
            strcpy( map[ y ][ x ].font, COLOR );     
            
            char color_value[ 5 ];                              // concatenate the new colored font.
            sprintf( color_value, "%dm",160 + target_count );
            strcat( map[ y ][ x ].font, color_value );             
            
            if( tag_row <= 25 )                                 // 2.Legend card: if haven't meet the card capacity limit( 20 tag and 18 targets ).
            {                                                   // move the new tag into legend card.
                map[ tag_row ][ 57 ].pixel = map[ y ][ x ].pixel;  
                strcpy( map[ tag_row ][ 57 ].font, map[ y ][ x ].font );
            
                char range_buffer[ 18 ];                        // set a range buffer to hold information of range of each tag.
                sprintf(range_buffer,"[%02d,%03d) [%02d,%03d)",
                        2 * ( x - 4 ),
                        2 * ( x - 4 ) + 2, 
                        4 * ( 26 - y ), 
                        4 * ( 26 - y ) + 4 );
                
                if( 2 * x - 6 == 100 )
                    range_buffer[ 7 ] = ']';                    // if meet 100, set as inclusive.           
                if( 108 - 4 * y == 100 )
                    range_buffer[ 16 ] = ']';
                                                                // calculate the range.
                for( int col = 60; col < 77; col++ )            // convert range info string into pixel map.
                    map[ tag_row ][ col ].pixel = range_buffer[ col - 60 ];
            }
            tag_row++;
        }

        if( target_count <= 18 )
        {
            char target_buffer[ 33 ];                           // set a buffer to hold information of each target.
            char target_name[ 10 ];                             // set a buffer to read 9 chars of target name.
            memmove(target_name, cur->name, 9);
                                                                // convert target info from target structure into string buffer.
            sprintf( target_buffer, "%-9s  %010f %010f", 
                     target_name, cur->latitude, cur->longtitude );
            
            if( cur->name[ 9 ] != '\0' )
                target_buffer[ 9 ] = target_buffer[ 10 ] = '.'; // if target's name exceeds 9 chars, use "..".
 
            if( target_count <= 9 )                             // left-hand side card.
            {
                map[ 29 + target_count ][ 3 ].pixel = map[ y ][ x ].pixel;
                strcpy( map[ 29 + target_count ][ 3 ].font, map[ y ][ x ].font );
                                                                // convert target info string into pixel map.
                for( int col = 6; col < 38; col++ )
                    map[ 29 + target_count ][ col ].pixel = target_buffer[ col - 6 ];
            }
            else                                                // right-hand side card.
            {
                map[ 20 + target_count ][ 42 ].pixel = map[ y ][ x ].pixel;
                strcpy( map[ 20 + target_count ][ 42 ].font, map[ y ][ x ].font );
                                                                // convert target info string into pixel map.
                for( int col = 45; col < 77; col++ )
                    map[ 20 + target_count ][ col ].pixel = target_buffer[ col - 45 ];  
            }
        }
          
        cur = cur->next;    
    }

    const wchar_t more[] = L"                        ..more..   ";
    if( tag_row > 25 )                                          // in case too many tags in the legend, use "more".
    { 
        for(int col = 57; col < 77; col++)
        {
            map[ 25 ][ col ].pixel = more[ col - 42 ];
            strcpy( map[ 25 ][ col ].font, BLINK );             // set the font blinking to prompt.
        }
    }
    
    if( target_count > 18 )                                     // in case too many targets in the target into card, use "more".
    { 
        for(int col = 42; col < 77; col++)
        {
            map[ 38 ][ col ].pixel = more[ col - 42 ];          // set the font blinking to prompt.
            strcpy( map[ 38 ][ col ].font, BLINK );
        }       
    }

//========================================= printout ==============================================================================
    for( int row = 0; row < 40; ++row )
    {
        for( int col = 0; col < 80; ++col )                     // printout all pixel with corresponding font.
            printf( "%s%lc%s", map[ row ][ col ].font, map[ row ][ col ].pixel, NORMAL );
        
        puts( "" );
    }
    setlocale( LC_ALL, "C" );                                   // change locale back.
    puts( "" );
}

