        .data
        .text
main:
        lui     $2, 0x1000
        addiu   $1, $1, 0x1
        sw      $1, 0($2)
        lw      $1, 0($2)
        subu    $4, $1, $5
