File types
----------

File formats taken from https://lucene.apache.org/core/2_9_4/fileformats.html.

| Name                  | Extension                | Brief Description                                                                                                       |
|:----------------------|:-------------------------|:------------------------------------------------------------------------------------------------------------------------|
| Segments File         | segments.gen, segments_N | Stores information about segments                                                                                       |
| Lock File             | write.lock               | The Write lock prevents multiple IndexWriters from writing to the same file.                                            |
| Compound File         | .cfs                     | An optional "virtual" file consisting of all the other index files for systems that frequently run out of file handles. |
| Fields                | .fnm                     | Stores information about the fields                                                                                     |
| Field Index           | .fdx                     | Contains pointers to field data                                                                                         |
| Field Data            | .fdt                     | The stored fields for documents                                                                                         |
| Term Infos            | .tis                     | Part of the term dictionary, stores term info                                                                           |
| Term Info Index       | .tii                     | The index into the Term Infos file                                                                                      |
| Frequencies           | .frq                     | Contains the list of docs which contain each term along with frequency                                                  |
| Positions             | .prx                     | Stores position information about where a term occurs in the index                                                      |
| Norms                 | .nrm                     | Encodes length and boost factors for docs and fields                                                                    |
| Term Vector Index     | .tvx                     | Stores offset into the document data file                                                                               |
| Term Vector Documents | .tvd                     | Contains information about each document that has term vectors                                                          |
| Term Vector Fields    | .tvf                     | The field level info about term vectors                                                                                 |
| Deleted Documents     | .del                     | Info about what files are deleted                                                                                       |

Fields within Synology NAS FTS:

| Name within Synology Lucene instance | Name within this library           | Description / format                              |
|:-------------------------------------|:-----------------------------------|:--------------------------------------------------|
| SYNODriveFileID                      | SynoTerms::driveFileID             | _empty_                                           |
| SYNODriveFileLabel                   | SynoTerms::driveFileLabel          | _empty_                                           |
| SYNODriveFileStar                    | SynoTerms::driveFileStar           | _empty_                                           |
| SYNOMDAcquisitionMake                | SynoTerms::acquisitionMake         | _empty_                                           |
| SYNOMDAcquisitionModel               | SynoTerms::acquisitionModel        | _empty_                                           |
| SYNOMDAttributeChangeDate            | SynoTerms::attributeChangeDate     | Unix epoch sec                                    |
| SYNOMDAuthors                        | SynoTerms::authors                 | _empty_                                           |
| SYNOMDCity                           | SynoTerms::city                    | _empty_                                           |
| SYNOMDContentModificationDate        | SynoTerms::contentModificationDate | _empty_                                           |
| SYNOMDContributors                   | SynoTerms::contributors            | _empty_                                           |
| SYNOMDCopyright                      | SynoTerms::copyright               | _empty_                                           |
| SYNOMDCountry                        | SynoTerms::country                 | _empty_                                           |
| SYNOMDCoverage                       | SynoTerms::coverage                | _empty_                                           |
| SYNOMDCreator                        | SynoTerms::creator                 | _empty_                                           |
| SYNOMDDateAdded                      | SynoTerms::dateAdded               | Unix epoch sec                                    |
| SYNOMDDescription                    | SynoTerms::description             | _empty_                                           |
| SYNOMDDisplayName                    | SynoTerms::displayName             | File name without path                            |
| SYNOMDDocInfo.SYNOMDPageLengthVector | SynoTerms::docInfo                 | _empty_                                           |
| SYNOMDExtension                      | SynoTerms::extension               | eg "docx"                                         |
| SYNOMDFSContentChangeDate            | SynoTerms::fsContentChangeDate     | Unix epoch sec                                    |
| SYNOMDFSCreationDate                 | SynoTerms::fsCreationDate          | Unix epoch sec                                    |
| SYNOMDFSName                         | SynoTerms::fsName                  | File name without path                            |
| SYNOMDFSSize                         | SynoTerms::fsSize                  | Size in bytes                                     |
| SYNOMDFinderOpenDate                 | SynoTerms::finderOpenDate          | Unix epoch sec                                    |
| SYNOMDHeadline                       | SynoTerms::headline                | _empty_                                           |
| SYNOMDIdentifier                     | SynoTerms::identifier              | _empty_                                           |
| SYNOMDIsDir                          | SynoTerms::isDir                   | String "y" / "n"                                  |
| SYNOMDKeywords                       | SynoTerms::keywords                | _empty_                                           |
| SYNOMDKind                           | SynoTerms::kind                    | eg "docx"                                         |
| SYNOMDLanguages                      | SynoTerms::languages               | _empty_                                           |
| SYNOMDLastUsedDateQ                  | SynoTerms::lastUsedDate            | Unix epoch sec                                    |
| SYNOMDLogicalSizeQ                   | SynoTerms::logicalSize             | Size in bytes                                     |
| SYNOMDOwnerGroupID                   | SynoTerms::ownerGroupID            | Unix GID                                          |
| SYNOMDOwnerUserID                    | SynoTerms::ownerUserID             | Unix UID                                          |
| SYNOMDParent                         | SynoTerms::parent                  | eg /volume1/sharename                             |
| SYNOMDPath                           | SynoTerms::path                    | Full path to file eg /volume1/sharename/file.docx |
| SYNOMDPrivilege                      | SynoTerms::privilege               | Unix privs string eg "rwxrwx---"                  |
| SYNOMDPublishers                     | SynoTerms::publishers              | _empty_                                           |
| SYNOMDRights                         | SynoTerms::rights                  | _empty_                                           |
| SYNOMDSearchAncestor                 | SynoTerms::searchAncestor          | _empty_                                           |
| SYNOMDSearchFileName                 | SynoTerms::searchFileName          | _empty_                                           |
| SYNOMDTextContent                    | SynoTerms::textContent             | Full text of document                             |
| SYNOMDTitle                          | SynoTerms::title                   | _empty_                                           |
| SYNOMDWildcard                       | SynoTerms::wildcard                | _empty_                                           |
| SYNOStateOrProvince                  | SynoTerms::stateOrProvince         | _empty_                                           |
| _SYNOMDFinderLabel                   | SynoTerms::sysFinderLabel          | eg "0"                                            |
| _SYNOMDGroupId                       | SynoTerms::sysGroupId              | Unix GID                                          |
| _SYNOMDUserTags                      | SynoTerms::sysUserTags             | _empty_                                           |