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

| Name within Synology Lucene instance | Name within this library | Description / format                                |
|:-------------------------------------|:-------------------------|:----------------------------------------------------|
| SYNODriveFileID                      | driveFileID              | _empty_                                             |
| SYNODriveFileLabel                   | driveFileLabel           | _empty_                                             |
| SYNODriveFileStar                    | driveFileStar            | _empty_                                             |
| SYNOMDAcquisitionMake                | acquisitionMake          | _empty_                                             |
| SYNOMDAcquisitionModel               | acquisitionModel         | _empty_                                             |
| SYNOMDAttributeChangeDate            | attributeChangeDate      | Unix epoch sec                                      |
| SYNOMDAuthors                        | authors                  | eg _Jane Doe_                                       |
| SYNOMDCity                           | city                     | _empty_                                             |
| SYNOMDContentModificationDate        | contentModificationDate  | Unix epoch sec                                      |
| SYNOMDContributors                   | contributors             | _empty_                                             |
| SYNOMDCopyright                      | copyright                | _empty_                                             |
| SYNOMDCountry                        | country                  | _empty_                                             |
| SYNOMDCoverage                       | coverage                 | _empty_                                             |
| SYNOMDCreator                        | creator                  | eg _Microsoft Office 2010_                          |
| SYNOMDDateAdded                      | dateAdded                | Unix epoch sec                                      |
| SYNOMDDescription                    | description              | _empty_                                             |
| SYNOMDDisplayName                    | displayName              | File name without path                              |
| SYNOMDDocInfo.SYNOMDPageLengthVector | docInfo                  | Character count per page eg `1280 1820` ...         |
| SYNOMDExtension                      | extension                | eg `docx`                                           |
| SYNOMDFSContentChangeDate            | fsContentChangeDate      | Unix epoch sec                                      |
| SYNOMDFSCreationDate                 | fsCreationDate           | Unix epoch sec                                      |
| SYNOMDFSName                         | fsName                   | File name without path                              |
| SYNOMDFSSize                         | fsSize                   | Size in bytes                                       |
| SYNOMDFinderOpenDate                 | finderOpenDate           | Unix epoch sec                                      |
| SYNOMDHeadline                       | headline                 | _empty_                                             |
| SYNOMDIdentifier                     | identifier               | _empty_                                             |
| SYNOMDIsDir                          | isDir                    | String `y` / `n`                                    |
| SYNOMDKeywords                       | keywords                 | _empty_                                             |
| SYNOMDKind                           | kind                     | eg `docx`                                           |
| SYNOMDLanguages                      | languages                | _empty_                                             |
| SYNOMDLastUsedDate                   | lastUsedDate             | Unix epoch sec                                      |
| SYNOMDLogicalSize                    | logicalSize              | Size in bytes                                       |
| SYNOMDOwnerGroupID                   | ownerGroupID             | Unix GID                                            |
| SYNOMDOwnerUserID                    | ownerUserID              | Unix UID                                            |
| SYNOMDParent                         | parent                   | eg `/volume1/sharename`                             |
| SYNOMDPath                           | path                     | Full path to file eg `/volume1/sharename/file.docx` |
| SYNOMDPrivilege                      | privilege                | Unix privs string eg `rwxrwx---`                    |
| SYNOMDPublishers                     | publishers               | _empty_                                             |
| SYNOMDRights                         | rights                   | _empty_                                             |
| SYNOMDSearchAncestor                 | searchAncestor           | _empty_                                             |
| SYNOMDSearchFileName                 | searchFileName           | _empty_                                             |
| SYNOMDTextContent                    | textContent              | Full text of document                               |
| SYNOMDTitle                          | title                    | _empty_                                             |
| SYNOMDWildcard                       | wildcard                 | _empty_                                             |
| SYNOStateOrProvince                  | stateOrProvince          | _empty_                                             |
| _SYNOMDFinderLabel                   | sysFinderLabel           | eg `0`                                              |
| _SYNOMDGroupId                       | sysGroupId               | Unix GID                                            |
| _SYNOMDUserTags                      | sysUserTags              | _empty_                                             |

Lucene version compatibility: v3.0.7.

[Luke 4.3.0](https://github.com/DmitryKey/luke/releases?page=4) can be used to read the Lucene++ indices.

## setup

- Runs in Docker
- Exposes search interface on port `18080`

### build locally

`docker build . -t synosearch`

### configure

To find relevant paths within volumeX:

`find /volumeX -maxdepth 1 -mindepth 1 -type d ! -name "@*"`

### deploy

**important** do not deploy this version on any production system: the `/get` endpoint allows all documents to be
returned with no permissions checks, and there is no filtering currently applied to the search.

To run and expose a share `/volume1/dropbox`, execute as follows:

```
nas$> docker run -p 18080:18080 \
  -v /volume1/dropbox:/volume1/dropbox:ro \
  -v /volume1/dropbox/@eaDir/SYNO@.fileindexdb:/indices/dropbox:ro \
  synosearch -index /indices/dropbox
```

Then you should be able to hit the `/search` endpoint:

`curl "http://hostname:18080/search?q=example" | jq '.'`

This will then return search results as JSON:

```
{
  "hits": [
    {
      "path": "/volume1/dropbox/ExampleNotificationPlugin.java",
      "fs_size": 995,
      "score": 1.72329,
      "extension": "java"
    }
  ],
  "total_hits": 1
}
```