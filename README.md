# License Client

 ## What's this?
License Client is a small library supposed to handle license operations (read, request, validate, etc) for an application.
***

## How it works?
The client installed on the application side will need to be fed with a license file to allow application usage.
The license file contains user-defined constraints for application usage. (name, version, expiry date, etc.)
The license file needs to be request from application provider side.
The data exchange related to license operations: load and request is protected by encryption algorithms according to Operation Mode.

At first use, client will generate it's own key that will be need to be provided to application provider side. This can be a file, a `POST` request or a socket payload depending on operation mode.
After license is created/generated on application provided side, this will be encrypted with client key and then forwarded back.

## Operation Modes:
The Client goal is having three operation modes:
- Standalone (Offline)
- Online
- License Manager (development ongoing)

### Security:
- An RSA algorithm is used for key exchange between Client and Application Owner
    - public key file should be delivered with application
    - private key file should be kept on provider side
- An AES algorithm is used to encrypt the content of the license file
    - Key is delivered via file which is encrypted with public key file
- A poor LSB stenographic algorithm is used to store the AES Key on the client side
    - Client key will be stored/read in/from a `png` picture

### Standalone Mode
#### Inputs:
 - License file (`.lic`)

The logic is represented by the following diagram:

```mermaid
sequenceDiagram
    participant User
    participant Owner

    Note over Owner: Generate RSA key pair
    Note over User, Owner: Public key included
    Owner->>User: Deliver application
    alt Client Init
        Note over User: Generate AES Key
        Note over User: Store key using stenography
        Note over User: Collect data (AES related)
        Note over User: Encrypt with Owner public key
    end
    User->>Owner: Send license request
    alt License Generation
        Note over Owner: Decrypt using private key
        Note over Owner: Generate .lic file
        Note over Owner: Encrypt with User AES Key
    end
    Owner->>User: Send .lic file
    note over User: Decrypt with AES Key
```
***

### Online Mode
#### Inputs:
 - License Server
 - Serial Number

Online mode use same logic as `Standalone` mode excepting that key exchange is done via `POST` request to the configured server.

### License Manager Mode (development ongoing)
#### Inputs:
 - License Manager host ip
 - License Manager port
 - License files (`.lic`)

The logic is represented by the following diagram:

```mermaid
sequenceDiagram
    participant User
    participant LicenseManager

    Note over User, LicenseManager: Generate AES Keys
    User->>LicenseManager: Request License (Acquire)
    alt Perferm key exchange (ECDH)
        Note over User: Generate ECDH Parameters
        User->>LicenseManager: Send X, Y Points
        Note over LicenseManager: Init/Compute ECDH Parameters
        LicenseManager->>User: Send Public Key
        Note over User, LicenseManager: Compute Kek (Key encryption key)
        Note over User, LicenseManager: Agree on shared Key
        Note over User: Generate IV
        User->>LicenseManager: Send IV
        User->>LicenseManager: Send Encrypted Client AES Key
    end
    Note over LicenseManager: Check for available license
    LicenseManager->>User: Respond with positive return code
    alt Deliver License
        Note over LicenseManager: Encrypt License with Client Key
        Note over LicenseManager: Encrypt License with Shared Key
    end
    LicenseManager->>User: Respond with encrypted license
    alt License Generation
        Note over User: Decrypt using Shared key
        Note over User: Decrypt using Client key
    end
```