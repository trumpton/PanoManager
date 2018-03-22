//    PanoManager - Interactive panorama tour manager program
//    Copyright (C) 2018  Steve M Clarke
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

//
// Error Messages
//

#ifndef PMERRORS_H
#define PMERRORS_H

static const char *_pmerrors_errstr[13] = {
    "Success",
    "Insufficient Memory",
    "Invalid Map Translation.  Try manually clearing out ~/.cache/PanoManager",
    "Internal Error - Invalid Pointer",
    "Internal Error - Input Not Defined",
    "Internal Error - Output Not Defined",
    "Internal Error - Invalid Target Image Size",
    "Unable to Write to Output File",
    "Unable to Read Equirectangular Image",
    "Unable to load Full-Size Face",
    "Unable to load Preview Face",
    "Unable to transfer resource files.  Check they are available in the lib folder",
    "Operation Cancelled"
} ;


class PM {

public:
    typedef enum {
        // Success
        Ok=0,

        // Allocation Failed
        OutOfMemory,

        // File corruptions
        InvalidMapTranslation,

        // Internal Errors
        InvalidPointer,

        // Passed parameter
        InputNotDefined,
        OutputNotDefined,
        InvalidTargetImageSize,

        // Error Loading / Saving
        OutputWriteError,
        EquirectReadError,
        FaceLoadError,
        PreviewLoadError,

        // Transfer Pannellum / Marzipano Files
        UnableToTransferResourceFiles,

        // Abort
        OperationCancelled

    } Err;

    static const char *errString(Err n) {
        if (n<Ok || n>OperationCancelled) return "" ;
        else return _pmerrors_errstr[(int)n] ;
    }
};



#endif // PMERRORS_H
