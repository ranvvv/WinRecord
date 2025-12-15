const fs = require('fs');


function main()
{
    let a=0;
    let b=0;
    let text = '';
    for(a=0;a<20;a++)
    {
        for(b=0;b<20;b++)
        {
            text += `void p${a.toString().padStart(3, '0')}_${b.toString().padStart(3, '0')}();\n`;
            text += `void p${a.toString().padStart(3, '0')}_${b.toString().padStart(3, '0')}_cleanup();\n`;
        }
        text += '\n\n';
    }

    console.log(text);
    fs.writeFileSync('order_function.h', text);
}

main();