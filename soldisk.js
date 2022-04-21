// === Soldisk === \\



immutable = {
  rarity : Abundant,
  int64 (TU) : born_time_unit,
  name : minted_to, // Required to be 
}

mutable = {
  name : Solar Disk,
  level: 12,
  img : ipfs_hash,
//  activated_by : holder_name,
  next_charge : time_unit,
  total_charges : 245, // Increments on a successful recharge
}
