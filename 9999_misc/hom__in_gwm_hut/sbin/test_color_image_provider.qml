import QtQuick 2.0

Item {
    anchors.fill: parent;

    Image {
        id: colorImageBg;

        width: 1280;
        height: 720;

        source: {
            "image://colors/white";
        }
    }
    
    Image {
        id: colorImage;

        property int a: 255;
        property int r: 0;
        property int g: 0;
        property int b: 0;

        width: 1280;
        height: 360;
    
        source: {
            var sColor;
            var sSource;

            sColor = "#" + getHexStrOfChar(a) + getHexStrOfChar(r) + getHexStrOfChar(g) + getHexStrOfChar(b);
            sSource = "image://colors/" + sColor;
            console.log("sColor=" + sColor);

            return sSource;
        }

        function getHexOfHalfChar(nHalfChar)
        {
            var sHalfChar = "0";

            switch (nHalfChar) {
            case 0:
                sHalfChar = "0";
                break;
            case 1:
                sHalfChar = "1";
                break;
            case 2:
                sHalfChar = "2";
                break;
            case 3:
                sHalfChar = "3";
                break;
            case 4:
                sHalfChar = "4";
                break;
            case 5:
                sHalfChar = "5";
                break;
            case 6:
                sHalfChar = "6";
                break;
            case 7:
                sHalfChar = "7";
                break;
            case 8:
                sHalfChar = "8";
                break;
            case 9:
                sHalfChar = "9";
                break;
            case 10:
                sHalfChar = "A";
                break;
            case 11:
                sHalfChar = "B";
                break;
            case 12:
                sHalfChar = "C";
                break;
            case 13:
                sHalfChar = "D";
                break;
            case 14:
                sHalfChar = "E";
                break;
            case 15:
                sHalfChar = "F";
                break;
            }

            return sHalfChar;
        }

        function getHexStrOfChar(nChar)
        {
            var sChar;
            var nRealChar;
            var nHalfChar1;
            var nHalfChar2;

            nRealChar = nChar % 256;
            nHalfChar1 = nRealChar % 16;
            nHalfChar2 = (nRealChar >> 4) % 16;

            sChar = getHexOfHalfChar(nHalfChar2) + getHexOfHalfChar(nHalfChar1);

            return sChar;
        }
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: {
            if (colorImage.r < 255) {
                colorImage.r++;
            } else {
                colorImage.r = 0;
                if (colorImage.g < 255) {
                    colorImage.g++;
                } else {
                    colorImage.g = 0;
                    if (colorImage.b < 255) {
                        colorImage.b++;
                    } else {
                        colorImage.b = 0;
                    }
                }
            }
        }
    }
}
