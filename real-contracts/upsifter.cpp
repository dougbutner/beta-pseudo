// --- Takes  --- \\

//NOTE This requires a variable `upstype`

// --- Assert the type of ups --- \\
up_type type_of_up = upstype;

uint32_t newsolups = 0; 
uint32_t newbluups = 0;
uint32_t newbigups = 0;

if (upstype == SOL){
  newsolups = upscount; 
} else if (upstype == BLUX){
  newbluups = upscount;
} else if (upstype == BIG){
  newbigups = upscount;
} else if (upstype == BIGSOL)
{
  if (upscount < 64){// Only send Ups  
    newsolups = upscount; 
  } else {
    newsolups = upscount; 
    newbigups = floor(upscount / 64); 
  }
}//END Upsifter Section
