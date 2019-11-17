#ifndef __Ax_Mail_MailPart_H__
#define __Ax_Mail_MailPart_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   15.11.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 20:09:21 +0100 (Mi, 20 Mär 2013) $
 *   $Revision: 1.10 $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/Globals.h>

//----- local -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     forward declarations
//=============================================================================
class MsgBody;

//=============================================================================
//     class MailPart
//=============================================================================
/** An instance of MailPart represents one portion of a mail.
 *
 *  Instances of MailPart can contain other instances of MailPart -> MailParts
 *  are organized in a tree, where every part has a parent part
 *  (except for the top-most part / the root) and any number of child parts.
 */
class MailPart
{
    //-------------------------------------------------------------------------
    //     friends
    //-------------------------------------------------------------------------
    /** Only Mail (and myself) should be allowed to create/destroy instances.
     *
     *  Since this lib currently only provides read-access to mailboxes, the
     *  creation of parts is done internally when reading the mails from
     *  the server. Therefore it's not necessary to allow the user of
     *  this lib to create mail parts.
     */
    friend class Mail;

  private:
    //-------------------------------------------------------------------------
    //     MailPart()
    //-------------------------------------------------------------------------
    /** Constructor
     *
     *  theBody is analyzed and child-parts are created if necessary.
     *
     * @param theMail        the mail this part belongs to
     * @param theParent      the parent part or 0 if this is the root
     * @param theBody        pointer to the c-client BODY-structure
     * @param theMailPartNum number of this part within the parents
     *                       collection of child-parts
     */
    MailPart( const Mail        *theMail
            , const MailPart    *theParent
            , const MsgBody     &theBody
            , const std::string &theSectionName
            );

    //-------------------------------------------------------------------------
    //     ~MailPart()
    //-------------------------------------------------------------------------
    /** Destructor
     *
     *  Any child parts contained within this part will be destryed.
     */
    ~MailPart();

  public:

    //-------------------------------------------------------------------------
    //     getMail()
    //-------------------------------------------------------------------------
    /** Returns the mail this part belongs to.
     *
     *  @returns the mail this part belongs to.
     */
    const Mail *getMail() const;

    //-------------------------------------------------------------------------
    //     getParent()
    //-------------------------------------------------------------------------
    /** Returns the parent part of this.
     *
     *  @returns the parent part of this or 0 if this is the root.
     */
    const MailPart *getParent() const;

    //-------------------------------------------------------------------------
    //     getSubPartCount()
    //-------------------------------------------------------------------------
    /** Returns the number of sub-parts.
     *
     *  @returns the number of sub-parts
     */
    int getSubPartCount() const { return mySubParts.size(); }

    //-------------------------------------------------------------------------
    //     getSubParts()
    //-------------------------------------------------------------------------
    /** Returns a collection of all sub-parts.
     *
     *  @returns a collection of all sub-parts.
     */
    const MailPartVector &getSubParts() const;

    //-------------------------------------------------------------------------
    //     getSubPart()
    //-------------------------------------------------------------------------
    /** Returns the sub-part with the given part-number.
     *
     *  @returns the sub-part with the given part-number or 0 if no
     *           child with thePartNum is present.
     */
    const MailPart *getSubPart(unsigned long thePartNum) const;

    //-------------------------------------------------------------------------
    //     getType()
    //-------------------------------------------------------------------------
    /** Returns the type code of this part.
     *
     *  @returns the type code of this part.
     */
    EBodyType getType() const;

    //-------------------------------------------------------------------------
    //     getTypeStr()
    //-------------------------------------------------------------------------
    /** Returns the type code of this part as string.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure. <br>
     *  Possible values are e.g. \c TYPETEXT, \c TYPEMULTIPART,
     *  \c TYPEMESSAGE and some others.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns the type code of this part as string.
     */
    std::string getTypeStr() const;

    //-------------------------------------------------------------------------
    //     getEnc()
    //-------------------------------------------------------------------------
    /** Returns the encoding of this part.
     *
     *  @returns the encoding of this part.
     */
    EBodyEncoding getEnc() const;

    //-------------------------------------------------------------------------
    //     getEncStr()
    //-------------------------------------------------------------------------
    /** Returns the encoding of this part as string.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure. <br>
     *  Possible values are e.g. \c ENC7BIT, \c ENC8BIT,
     *  \c ENCQUOTEDPRINTABLE and some others.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns the encoding of this part as string.
     */
    std::string getEncStr() const;

    //-------------------------------------------------------------------------
    //     getCountLines()
    //-------------------------------------------------------------------------
    /** Returns the number of lines in the body of this part.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns the number of lines in the body of this part.
     */
    unsigned long getCountLines() const;

    //-------------------------------------------------------------------------
    //     getCountBytes()
    //-------------------------------------------------------------------------
    /** Returns the number of bytes in the body of this part.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns the number of bytes in the body of this part.
     */
    unsigned long getCountBytes() const;

    //-------------------------------------------------------------------------
    //     getSubType()
    //-------------------------------------------------------------------------
    /** Returns the subtype string from the body of this part.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns the subtype string from the body of this part.
     */
    std::string getSubType() const;

    //-------------------------------------------------------------------------
    //     getID()
    //-------------------------------------------------------------------------
    /** Returns the id string from the body of this part.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns the id string from the body of this part.
     */
    std::string getID() const;

    //-------------------------------------------------------------------------
    //     getDesc()
    //-------------------------------------------------------------------------
    /** Returns the description string from the body of this part.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns the description string from the body of this part.
     */
    std::string getDesc() const;

    //-------------------------------------------------------------------------
    //     getMD5()
    //-------------------------------------------------------------------------
    /** Returns the content md5 checksum from the body of this part.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns the content md5 checksum from the body of this part.
     */
    std::string getMD5() const;

    //-------------------------------------------------------------------------
    //     getParams()
    //-------------------------------------------------------------------------
    /** Returns a dictionary with the parameters of the body of this part.
     *
     *  This method simply returns the value of the corresponding
     *  member of the \c c-client BODY-structure.
     *
     *  \see \ref c_client for further information.
     *
     *  @returns a dictionary with the parameters of the body of this part.
     */
    String2StringMap getParams() const;

    //-------------------------------------------------------------------------
    //     getParam()
    //-------------------------------------------------------------------------
    /** Returns true if the given parameter is present and the value of the
     *  parameter in the second argument.
     *
     *  @param theParam parameter to search
     *  @param fFound true if the given parameter is present.
     *  @returns the value of the parameter (if fFount values is true, "" otherwise)
     */
    std::string getParam(std::string theParam, bool *fFound = 0) const;

    //-------------------------------------------------------------------------
    //     getBodyText()
    //-------------------------------------------------------------------------
    /** Returns the body text of this part.
     *
     *  If the part is encoded in "quoted-printable" this method returns
     *  the decoded text.
     *
     *  @returns the body text of this part.
     */
    std::string getBodyText() const;

    //-------------------------------------------------------------------------
    //     getPartName()
    //-------------------------------------------------------------------------
    /** Returns the name of this part which can be used internally for
     *  later mail_fetchpart operations.
     *
     *  Names look e.g. like \c "1", \c "2.1", etc.
     *
     *  Attention: for mail parts with isMultiMailPart() == TRUE this
     *  method will return ""!
     *
     *  @returns the name of this part.
     */
    std::string getPartName() const;

    //-------------------------------------------------------------------------
    //     isMultiPart()
    //-------------------------------------------------------------------------
    /** Returns whether this part is a multi-part ((getType() == TYPEMULTIPART)
     *
     *  @returns whether this part is a multi-part ((getType() == TYPEMULTIPART)
     */
    bool isMultiPart() const;

    //=========================================================================
    //     commands to the mail server
    //=========================================================================

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    /** the mail this part belongs to
     */
    const Mail       *myMail;

    /** the parent part of this if this is a nested part
     *  or 0 if this is the 'root'-part
     */
    const MailPart   *myParent;

    /** Pointer to the c-client body structure
     */
    MsgBody          *myBody;

    /** absolute number-string of the part
     */
    std::string      mySectionName;

    /** Collection of parts contained within this
     */
    MailPartVector   mySubParts;

}; // class MailPart

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif

