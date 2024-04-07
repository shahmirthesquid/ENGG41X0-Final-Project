import React, {useState, useEffect} from "react";

import styled from "styled-components";

import { ControlItem } from "./ControlItem";
import { DisplayItem } from "./DisplayItem";

import Config from "./../configuration.json";
let loc;
if (Config.find(entry => entry.name === "language")) {
    loc = require("./../lang/" + Config.find(entry => entry.name === "language").value + ".json");
} else {
    loc = require("./../lang/en.json");
}

const Grey = styled.span`
    color:#666;
    white-space: nowrap;
`;

const Display = styled.p`
    span, & > div {
        border-radius:3px;
        padding:0.3em;
        display:inline-block;
        border:1px solid #c0d1de;
        background-color:#edf3fc;
    }
    span.false {
        border:1px solid #ff3333;
        background-color:#ffb3b3;
    }
    span.true {
        border:1px solid #c4e052;
        background-color:#e6f9b8;
    }
    span.Off {
        border:1px solid #ff3333;
        background-color:#ffb3b3;
    }
    span.On {
        border:1px solid #c4e052;
        background-color:#e6f9b8;
    }

    & > div {
        width:437px;
        max-width:calc(100% - 0.6em);
    }

    label {
        vertical-align:top;
    }
`;

const Control = styled.p`
    input[type=number],    
    input[type=password],
    input[type=text] {
        margin-right:0.3em;
        width:410px;
        max-width:calc(100% - 40px);
    }

    button {        
        padding:0.4em 0.5em;
    }
`;

const Layout = styled.div`
    * {
        width:calc(660px + 3em);
        margin-left:0px;
        max-width:calc(100% - 40px);
    }
`;

const DefaultTypeAttributes = {
    char: {
        type: "text",
    },
    color: {
        type: "color",
    },
    time: {
        type: "time",
    },
    bool: {
        type: "checkbox",
    },
    uint8_t: {
        type: "number",
        min: 0,
        max: 255,
        step: 1,
    },
    int8_t: {
        type: "number",
        min: -128,
        max: 127,
        step: 1,
    },
    uint16_t: {
        type: "number",
        min: 0,
        max: 65535,
        step: 1,
    },
    int16_t: {
        type: "number",
        min: -32768,
        max: 32767,
        step: 1,
    },
    uint32_t: "number",
    int32_t: {
        type: "number",
        min: -2147483648,
        max: 2147483647,
        step: 1,
    },
    float: {
        type: "number",
        min: -3.4028235E+38,
        max: 3.4028235E+38,
        step: "any",
    },
};

export function DashboardItems(props) {

    const [data, setData] = useState([]);
    
    //populate graphs
    useEffect(() => {   
        if (props.data.length > 0 && typeof props.data[0][1] !== "undefined") {
            //contains historical data
            setData(props.data[props.data.length - 1][1]);
        } else {
            setData(props.data);
        }        
    });

    
    let confItems;
    if (props.items.length == 0) {
        confItems = <p>{loc.dashEmpty}</p>;
    } else {

        let outletCount = data.outletCount; /*create local variable with the number of outlets to be shown*/

        for (let i = 0; i < props.items.length; i++) {
            
            if (typeof props.items[i].name === "string" && props.items[i].name.length >= 2){
                let lastTwoChars = props.items[i].name.slice(-2)
                /*
                console.log("checking item", props.items[i].name);
                console.log("last two chars are", lastTwoChars);
                console.log("is integer?", Number.isInteger(Number(lastTwoChars)));
                console.log("> outletCount?", Number(lastTwoChars)>outletCount);
                */
                if(Number.isInteger(Number(lastTwoChars)) && Number(lastTwoChars)>outletCount){
                    /*console.log("IS AN INTEGER and > outletCount!!!")*/
                    props.items[i].hidden = true;
                }
            }

            if (props.items[i].hidden) {
                continue;
            }

            if (props.items[i].type == "separator") {
                confItems = <>{confItems}<Layout><hr /></Layout></>;
                continue;
            }            

            if (props.items[i].type == "header") {
                confItems = <>{confItems}<Layout><h3>{props.items[i].text}</h3></Layout></>;
                continue;
            }            

            if (props.items[i].type == "label") {
                confItems = <>{confItems}<Layout><p>{props.items[i].text}</p></Layout></>;
                continue;
            }

            if (props.items[i].type == "nolabel") {
                confItems = <>{confItems}<Layout><p></p></Layout></>;
                continue;
            }

            let value;
            if (typeof data !== "undefined" && typeof data[props.items[i].name] !== "undefined") { 
                value = data[props.items[i].name]; 

                //number of digits
                if (props.items[i].type == "float" && typeof props.items[i].digits !== "undefined") {
                    value = parseFloat(value).toFixed(props.items[i].digits);
                }
                
            } else { value = ""; }

            const configInputAttributes = DefaultTypeAttributes[props.items[i].type] || {};
            let inputType;
            if (typeof props.items[i].control !== "undefined") {
                inputType = props.items[i].control;
            } else {
                inputType = DefaultTypeAttributes[props.items[i].type].type || "text";
            }

            const conditionalAttributes = {};
            let rangeInfo;

            switch (inputType) {
                case "text":
                    conditionalAttributes.maxlength = props.items[i].length - 1;
                    break;

                case "checkbox":
                    conditionalAttributes.checked = value;
                    break;

                case "number":
                    conditionalAttributes.min = props.items[i].min; // || configInputAttributes.min;
                    conditionalAttributes.max = props.items[i].max; // || configInputAttributes.max;
                    conditionalAttributes.step = props.items[i].step; // || configInputAttributes.step;

                    if (typeof conditionalAttributes.min !== "undefined") {
                        rangeInfo = <>
                            <Grey>({conditionalAttributes.min} &ndash; {conditionalAttributes.max})</Grey>
                        </>;
                    }
                    break;

                case "slider":
                    conditionalAttributes.min = props.items[i].min || configInputAttributes.min;
                    conditionalAttributes.max = props.items[i].max || configInputAttributes.max;
                    conditionalAttributes.step = props.items[i].step || configInputAttributes.step;                   
                    break;

                case "select":
                    conditionalAttributes.options = props.items[i].options;                                        
                    conditionalAttributes.optionLabels = props.items[i].options;
                    if (typeof props.items[i].optionLabels !== "undefined") {
                        conditionalAttributes.optionLabels = props.items[i].optionLabels;
                    }
                    if(props.items[i].name.slice(-7,-2) == "Mode_"){ /* if we are on a selection, check which of the 16 relays it is selecting for */
                        let relayNumber = props.items[i].name.slice(-2); /* creates a string with the first character as '0' then appends the number and takes the last two digits. This makes 1 = 01 and 16 still = 16. This variable allows the slice to only be performed once */
                        /*console.log("relayNumber",relayNumber);*/
                        if(props.items[i].name.slice(-7) == "Mode_"+ relayNumber  ){ /* is it mode01 through mode16+?  */
                            /*alert(props.items[i].name.slice(-5)+ " : " + "Mode"+ j.toString());*/
                            /*alert("option:"+props.items[i].option+", "+ "options:" +props.items[i].option);*/
                            /*alert(props.items[i]);*/
                            /*console.log("data items:",data);*/
                            /*console.log("props:",props);*/
                            /*console.log("items", props.items);*/
                            /*
                            const tmpName = props.items[i].name;
                            console.log("propName: ",props.items[i].name);
                            console.log("tmpName: ", tmpName);
                            console.log("dynamicAccess: ",data[tmpName]);
                            console.log("directAccess: ",data[props.items[i].name]);
                            */
                            
                            if(data[props.items[i].name] == "None" || data[props.items[i].name] == "undefined"){ /* if it is a mode is it set to None? */
                                for (let k = 0; k < props.items.length; k++){ /* if it is set to none go through all items to find the ones that need to be hidden*/
                                    /*console.log("Looping through # of items", props.items.length);*/
                                    if (typeof props.items[k].name === "string" && props.items[k].name.length >= 9) {
                                
                                        
                                        let nameSlice = props.items[k].name.slice(-9); /* takes the last 9 characters of the string. 7 characters are for the keyword, and 2 characters are for the relay number. This is set to a variable so that the slice only needs to performed once */
                                        /*
                                        console.log("looking at: ", props.items[k].name);
                                        console.log("comparing slice: ",nameSlice +" against " + "_daily_" + relayNumber );*/
                                        
                                        if(nameSlice == "_daily_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                        else if(nameSlice == "_intrv_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                        else if(nameSlice == "_weekl_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                    }
                                }
                            }

                            else if(data[props.items[i].name] == "Daily"){ /* if it is a mode is it set to Daily? */
                                for (let k = 0; k < props.items.length; k++){ /* if it is set to none go through all items to find the ones that need to be hidden*/
                                    if (typeof props.items[k].name === "string" && props.items[k].name.length >= 9) {
                                
                                        /*console.log("looking at: ", props.items[k].name);*/
                                        /*console.log("comparing slice: ",props.items[k].name.slice(-6) +" against " + "_daily_" + j.toString() );*/

                                        let nameSlice = props.items[k].name.slice(-9); /* takes the last 9 characters of the string. 7 characters are for the keyword, and 2 characters are for the relay number. This is set to a variable so that the slice only needs to performed once */
                                        if(nameSlice == "_daily_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = false;
                                        }
                                        else if(nameSlice == "_intrv_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                        else if(nameSlice == "_weekl_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                    }
                                } 
                            }

                            else if(data[props.items[i].name] == "Time Interval"){ /* if it is a mode is it set to Daily? */
                                for (let k = 0; k < props.items.length; k++){ /* if it is set to none go through all items to find the ones that need to be hidden*/
                                    if (typeof props.items[k].name === "string" && props.items[k].name.length >= 9) {
                                
                                        /*console.log("looking at: ", props.items[k].name);*/
                                        /*console.log("comparing slice: ",props.items[k].name.slice(-6) +" against " + "_daily_" + j.toString() );*/

                                        let nameSlice = props.items[k].name.slice(-9); /* takes the last 9 characters of the string. 7 characters are for the keyword, and 2 characters are for the relay number. This is set to a variable so that the slice only needs to performed once */
                                        if(nameSlice == "_daily_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                        else if(nameSlice == "_intrv_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = false;
                                        }
                                        else if(nameSlice == "_weekl_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                    }
                                }
                            }

                            else if(data[props.items[i].name] == "Weekly"){ /* if it is a mode is it set to Daily? */
                                for (let k = 0; k < props.items.length; k++){ /* if it is set to none go through all items to find the ones that need to be hidden*/
                                    if (typeof props.items[k].name === "string" && props.items[k].name.length >= 9) {
                                
                                        /*console.log("looking at: ", props.items[k].name);*/
                                        /*console.log("comparing slice: ",props.items[k].name.slice(-6) +" against " + "_daily_" + j.toString() );*/

                                        let nameSlice = props.items[k].name.slice(-9); /* takes the last 9 characters of the string. 7 characters are for the keyword, and 2 characters are for the relay number. This is set to a variable so that the slice only needs to performed once */
                                        if(nameSlice == "_daily_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                        else if(nameSlice == "_intrv_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = true;
                                        }
                                        else if(nameSlice == "_weekl_" + relayNumber ) { /* found related item, check if it is matching number*/
                                            props.items[k].hidden = false;
                                        }
                                    }
                                }
                            }
                            
                            
                        }

                    }
                    break;
            }

            const direction = props.items[i].direction || "config";
            
            switch (direction) {
                case "display":                    
                    confItems = <>{confItems}
                        <Display>
                            <label htmlFor={props.items[i].name}><b>{props.items[i].label || props.items[i].name}</b>: {rangeInfo}</label>
                            <DisplayItem 
                                item={props.items[i]} 
                                data={props.data}
                                value={value} />
                        </Display>
                    </>;
                    
                    break;
                case "display_OnOff":                    
                confItems = <>{confItems}
                    <Display>
                        <label htmlFor={props.items[i].name}><b>{props.items[i].label || props.items[i].name}</b>: {rangeInfo}</label>
                        <DisplayItem 
                            item={props.items[i]} 
                            data={props.data}
                            value={value? "On" : "Off"} />
                    </Display>
                </>;
                
                break;

                case "display_noLabel_OnOff":                    
                confItems = <>{confItems}
                    <Display>
                        {/*<label htmlFor={props.items[i].name}><b>{props.items[i].label || props.items[i].name}</b>: {rangeInfo}</label>*/}
                        <DisplayItem 
                            item={props.items[i]} 
                            data={props.data}
                            value={value? "On" : "Off"} />
                    </Display>
                </>;
                
                break;

                case "display_noLabel":                    
                confItems = <>{confItems}
                    <Display>
                        {/* <label htmlFor={props.items[i].name}><b>{props.items[i].label || props.items[i].name}</b>: {rangeInfo}</label> */}
                        <DisplayItem 
                            item={props.items[i]} 
                            data={props.data}
                            value={value} />
                    </Display>
                </>;
                
                break;

                case "control":                     
                    confItems = <>{confItems}
                        <Control>
                            <label htmlFor={props.items[i].name}><b>{props.items[i].label || props.items[i].name}</b>: {rangeInfo}</label>
                            <ControlItem 
                                API={props.API} 
                                dataType={props.items[i].type} 
                                type={inputType} 
                                name={props.items[i].name} 
                                value={value} 
                                conditionalAttributes={conditionalAttributes} 
                            />                            
                        </Control>
                    </>;
                    break;

                case "config":
                    if (inputType == "select") {
                        let options;
                        for (let i = 0; i < conditionalAttributes.options.length; i++) {  
                            let label = conditionalAttributes.options[i];
                            if (typeof conditionalAttributes.optionLabels[i] !== "undefined") {
                                label = conditionalAttributes.optionLabels[i];
                            }
                            options = <>{options}<option value={conditionalAttributes.options[i]}>{label}</option></>;            
                        }
                        confItems = <>{confItems}
                            <p>
                                <label htmlFor={props.items[i].name}><b>{props.items[i].label || props.items[i].name}</b>: {rangeInfo}</label>
                                <select id={props.items[i].name} name={props.items[i].name} value={value} disabled={props.items[i].disabled}>
                                    {options}
                                </select>
                            </p>
                        </>;
                    } else if (inputType == "slider") {
                        
                        const [rangeval, setRangeval] = useState(null);
                        
                        confItems = <>{confItems}
                            <p>
                                <label htmlFor={props.items[i].name}><b>{props.items[i].label || props.items[i].name}</b>:</label>
                                <input type="range" id={props.items[i].name} name={props.items[i].name} value={rangeval || value} {...conditionalAttributes} disabled={props.items[i].disabled} onInput={(event) => setRangeval(event.target.value)} />
                                <output>{rangeval || value}</output>
                            </p>
                        </>;
                    } else {
                        confItems = <>{confItems}
                            <p>
                                <label htmlFor={props.items[i].name}><b>{props.items[i].label || props.items[i].name}</b>: {rangeInfo}</label>
                                <input type={inputType} id={props.items[i].name} name={props.items[i].name} value={value} {...conditionalAttributes} disabled={props.items[i].disabled} />
                            </p>
                        </>;
                    }
                    break;
                    
            }
            
        }
    }

    return confItems;

}