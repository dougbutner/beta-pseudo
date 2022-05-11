
// --- Council Contract --- \\

Each geodomain has default "template 1" that includes dominal functions. 

Any User can join this council. 
Council members can add proposals.
Proposals are voted on by council members 

council.cxc { 
  ACTION addproposal({})
  ACTION removeproposal(account, proposalid)
  ACTION editproposal(proposalid, {changes})
  
  ACTION voteproposal()
  
  ACTION addproposalformat({})
  ACTION removeproposalformat(account, proposalid)
  ACTION editproposalformat(proposalid, {changes}) // Checks against given format t

  ACTION publishlist({},private,activate_also)
  ACTION removelist()
  
  ACTION makecouncil()
  ACTION makecounciltemplate()
  ACTION joincouncil()
  ACTION leavecouncil()
  
  ACTION publishreport()
  ACTION publishcouncilreport()
  
  ACTION votecouncilchief()
  ACTION votecouncil()
  
  
  
  
  TABLE councils [
    geotribe
    council type 
    
  ]
  
  TABLE votelist [
    
  ]
  
}


Project Object

{
  "title": "Title of the Grant",
  "description": "IPFS of longer description?",
  "media [txt/array]": ,
  "steps": {
    "Name of Step",
    "Descrition of step"
  },
  "metrics": {
    name_of_metric: "",
    value_of_metric: TYPE_OF_METRIC, // Store type to later require on input
  },

  "links": [links relevant to the pojects],
  "leaders": [account names of those in charge who can later publish reports], 
  "team-roles": {
    "Name of Role": ,
    "Description of Role": 
  },
  "roled-members" {
    "Account": 
    "Name of role": 
  }
}

Report Object 

{
  "report-title": "Title of the Grant", //?? Perhaps a generated title using the name + "2022Q1" or similar
  "description": "IPFS of longer description?",
  "performance-against-metrics": , // MUST match the type of data required 
}
