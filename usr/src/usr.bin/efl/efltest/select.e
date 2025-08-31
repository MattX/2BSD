procedure s()
integer x
integer k

select(x)
    {
    case 1:
        k = 1
    case 3,5:
        k = 2
    default:
        k = 3
    }
end
